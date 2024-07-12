#include "DomTree.hh"

#include "Function.hh"

vector<BasicBlock *> dfsPred; // fth
vector<int> dsTree;           // fa
vector<int> sDom;             // sdom
vector<int> mn;
vector<vector<int>> sDomSucc;

void DomTree::dfs(BasicBlock *node, int &d, CFG *cfg) {
  dfnToBB.push_back(node);
  bbToDfn[node] = d++;
  for (BasicBlock *succ : *cfg->getSuccOf(node)) {
    if (bbToDfn.find(succ) == bbToDfn.end()) {
      dfsPred[d] = node;
      dfs(succ, d, cfg);
    }
  }
}

int find(int x) {
  if (dsTree[x] == x) {
    return x;
  }
  int tmp = dsTree[x];
  dsTree[x] = find(dsTree[x]);
  if (sDom[mn[tmp]] < sDom[mn[x]]) {
    mn[x] = mn[tmp];
  }
  return dsTree[x];
}

void DomTree::buildDomTree() {
  int size = blocks->getSize();
  dfnToBB.clear();
  dfnToBB.reserve(size);

  bbToDfn.clear();

  dfsPred = vector<BasicBlock *>(size);

  dsTree.clear();
  dsTree.reserve(size);

  sDom.clear();
  sDom.reserve(size);

  mn.clear();
  mn.reserve(size);

  sDomSucc = vector<vector<int>>(size, vector<int>());

  iDom = vector<int>(size);

  int depth = 0;
  dfs(cfg->getEntry(), depth, cfg);
  for (int i = 0; i < size; i++) {
    dsTree.push_back(i);
    sDom.push_back(i);
    mn.push_back(i);
  }

  for (int i = depth - 1; i >= 1; i--) {
    BasicBlock *bb = dfnToBB[i];
    int res = i;
    for (BasicBlock *pred : *cfg->getPredOf(bb)) {
      auto item = bbToDfn.find(pred);
      if (item == bbToDfn.end()) {
        // Unuse block
        continue;
      }
      int pDfn = item->second;
      find(pDfn);
      if (pDfn < i) {
        res = std::min(res, pDfn);
      } else {
        res = std::min(res, sDom[mn[pDfn]]);
      }
    }
    sDom[i] = res;
    dsTree[i] = bbToDfn[dfsPred[i]];
    sDomSucc[sDom[i]].push_back(i);

    int u = bbToDfn[dfsPred[i]];
    for (int uSucc : sDomSucc[u]) {
      find(uSucc);
      if (sDom[mn[uSucc]] == u) {
        iDom[uSucc] = u;
      } else {
        iDom[uSucc] = mn[uSucc];
      }
    }
    sDomSucc[u].clear();
  }
  for (int i = 1; i < size; i++) {
    if (iDom[i] != sDom[i]) {
      iDom[i] = iDom[iDom[i]];
    }
    setDominator(dfnToBB[i], dfnToBB[iDom[i]]);
    domChildren[dfnToBB[iDom[i]]]->pushBack(dfnToBB[i]);
  }
  dtActive = true;
}

DomTree::DomTree(Function *func) : dtActive(false) {
  blocks = func->getBasicBlocks();
  cfg = func->getCFG();
  assert(cfg);
  for (BasicBlock *bb : *blocks) {
    domChildren[bb] = new LinkedList<BasicBlock *>();
  }
}

void DomTree::draw() {
  vector<std::pair<string, string>> edges;
  for (auto &item : dominators) {
    edges.push_back({item.second->getName(), item.first->getName()});
  }
  visualizeGraph(edges);
}

void DomTree::calculateDF() {
  assert(cfg);
  assert(blocks);
  if (!dtReady()) {
    buildDomTree();
  }
  for (BasicBlock *bb : *blocks) {
    dominanceFrontier[bb] = new LinkedList<BasicBlock *>();
  }
  for (BasicBlock *bb : *blocks) {
    if (cfg->getPredOf(bb)->getSize() > 1) {
      for (BasicBlock *pred : *cfg->getPredOf(bb)) {
        BasicBlock *runner = pred;
        BasicBlock *idom = getDominator(bb);
        while (runner != idom) {
          dominanceFrontier[runner]->pushBack(bb);
          runner = getDominator(runner);
        }
      }
    }
  }
  dfActive = true;
#ifdef DEBUG_MODE
  for (BasicBlock *bb : *blocks) {
    std::cout << bb->getName() << ": ";
    for (BasicBlock *df : *getDF(bb)) {
      std::cout << df->getName() << " ";
    }
    std::cout << std::endl;
  }
#endif
}

void DomTree::calculateIDF(BBListPtr src, BBListPtr result) {
  unordered_set<BasicBlock *> resultSet;
  queue<BasicBlock *> worklist;
  for (BasicBlock *bb : *src) {
    for (BasicBlock *df : *getDF(bb)) {
      resultSet.insert(df);
      worklist.push(df);
    }
  }
  while (!worklist.empty()) {
    BasicBlock *bb = worklist.front();
    worklist.pop();
    for (BasicBlock *df : *getDF(bb)) {
      if (resultSet.count(df) == 0) {
        resultSet.insert(df);
        worklist.push(df);
      }
    }
  }
  for (BasicBlock *bb : resultSet) {
    result->pushBack(bb);
  }
}

void DomTree::mergeChildrenTo(BasicBlock *src, BasicBlock *dest) {
  BBListPtr srcList = domChildren[src];
  BBListPtr destList = domChildren[dest];
  for (BasicBlock *domChild : *srcList) {
    destList->pushBack(domChild);
    setDominator(domChild, dest);
  }
  domChildren.erase(src);
}

void DomTree::preOrderTraversal(BasicBlock *node,
                                std::vector<BasicBlock *> &result) {
  result.push_back(node);
  for (BasicBlock *child : *getDomChildren(node)) {
    preOrderTraversal(child, result);
  }
}

std::vector<BasicBlock *> DomTree::getDomTreePreOrder() {
  std::vector<BasicBlock *> result;
  preOrderTraversal(cfg->getEntry(), result);
  return result;
}

void DomTree::testDomTree() {

  cfg = new CFG();
  BasicBlock *R = new BasicBlock("R", true);
  BasicBlock *A = new BasicBlock("A", true);
  BasicBlock *B = new BasicBlock("B", true);
  BasicBlock *C = new BasicBlock("C", true);
  BasicBlock *D = new BasicBlock("D", true);
  BasicBlock *E = new BasicBlock("E", true);
  BasicBlock *F = new BasicBlock("F", true);
  BasicBlock *G = new BasicBlock("G", true);
  BasicBlock *H = new BasicBlock("H", true);
  BasicBlock *I = new BasicBlock("I", true);
  BasicBlock *J = new BasicBlock("J", true);
  BasicBlock *K = new BasicBlock("K", true);
  BasicBlock *L = new BasicBlock("L", true);

  cfg->addNode(R);
  cfg->addNode(A);
  cfg->addNode(B);
  cfg->addNode(C);
  cfg->addNode(D);
  cfg->addNode(E);
  cfg->addNode(F);
  cfg->addNode(G);
  cfg->addNode(H);
  cfg->addNode(I);
  cfg->addNode(J);
  cfg->addNode(K);
  cfg->addNode(L);

  cfg->addEdge(R, A);
  cfg->addEdge(R, B);
  cfg->addEdge(R, C);
  cfg->addEdge(R, K);

  cfg->addEdge(A, D);
  cfg->addEdge(B, A);
  cfg->addEdge(B, D);
  cfg->addEdge(B, E);

  cfg->addEdge(C, G);
  cfg->addEdge(C, F);

  cfg->addEdge(D, L);

  cfg->addEdge(E, H);

  cfg->addEdge(F, I);

  cfg->addEdge(G, I);
  cfg->addEdge(G, J);

  cfg->addEdge(H, E);
  cfg->addEdge(H, K);

  cfg->addEdge(I, K);

  cfg->addEdge(J, I);

  cfg->addEdge(K, R);
  cfg->addEdge(K, I);

  cfg->addEdge(L, H);

  cfg->setEntry(R);
  cfg->setExit(K);

  blocks = cfg->getBlocks();

  buildDomTree();

  for (auto &item : dominators) {
    std::cout << item.first->getName() << ": " << item.second->getName()
              << std::endl;
  }
  dtActive = true;
  draw();
  calculateDF();
}