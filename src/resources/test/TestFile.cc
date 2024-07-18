#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

// 定义一个简单的测试框架
class TestFramework {
public:
    TestFramework() : totalTests(0), passedTests(0) {}

    // 运行指定目录下所有的 .sy 文件中的测试，并与对应的 .out 文件比较
    void runTestsInDirectory(const std::string& directoryPath) {
        std::vector<std::string> fileNames = listFilesInDirectory(directoryPath, ".sy");

        for (const auto& fileName : fileNames) {
            std::string inputFilePath = directoryPath + "/" + fileName;
            std::string expectedOutputFilePath = directoryPath + "/" + getExpectedOutputFileName(fileName);

            std::cout << "Running tests for input file: " << fileName << "\n";
            bool passed = runSingleTest(inputFilePath, expectedOutputFilePath);
            if (passed) {
                passedTests++;
            }
            totalTests++;

            std::cout << "\n";
        }

        summarizeTests();
    }

private:
    int totalTests;
    int passedTests;

    // 辅助函数：列出指定目录下的所有指定后缀的文件
    std::vector<std::string> listFilesInDirectory(const std::string& directory, const std::string& extension) {
        std::vector<std::string> fileNames;
        std::ifstream listFile(directory + "/file_list.txt");  // 假设有一个文件列表文件，记录了要测试的.sy文件名

        std::string fileName;
        while (std::getline(listFile, fileName)) {
            if (fileName.size() >= extension.size() && fileName.compare(fileName.size() - extension.size(), extension.size(), extension) == 0) {
                fileNames.push_back(fileName);
            }
        }

        return fileNames;
    }

    // 辅助函数：根据输入文件名获取预期输出文件名
    std::string getExpectedOutputFileName(const std::string& inputFileName) {
        // 假设输入文件名是 input1.sy，对应的预期输出文件名是 input1.out
        return inputFileName.substr(0, inputFileName.find_last_of('.')) + ".out";
    }

    // 运行单个测试，比较实际输出与预期输出
    bool runSingleTest(const std::string& inputFileName, const std::string& expectedOutputFileName) {
        std::ifstream inputFile(inputFileName);
        std::ifstream expectedOutputFile(expectedOutputFileName);

        if (!inputFile.is_open() || !expectedOutputFile.is_open()) {
            std::cerr << "Error: Failed to open file " << inputFileName << " or " << expectedOutputFileName << "\n";
            return false;
        }

        std::stringstream actualOutput;
        std::string inputLine;
        while (std::getline(inputFile, inputLine)) {
            // 模拟处理输入，这里简单地将输入写入 actualOutput
            actualOutput << inputLine << "\n";
        }

        std::stringstream expectedOutput;
        std::string expectedLine;
        while (std::getline(expectedOutputFile, expectedLine)) {
            // 模拟处理预期输出，这里简单地将预期输出写入 expectedOutput
            expectedOutput << expectedLine << "\n";
        }

        inputFile.close();
        expectedOutputFile.close();

        // 比较实际输出和预期输出
        if (actualOutput.str() == expectedOutput.str()) {
            std::cout << "[PASS] Test passed for " << inputFileName << "\n";
            return true;
        } else {
            std::cout << "[FAIL] Test failed for " << inputFileName << "\n";
            return false;
        }
    }

    // 辅助函数：打印测试总结
    void summarizeTests() {
        std::cout << "\n\n";
        std::cout << "Total tests: " << totalTests << "\n";
        std::cout << "Passed tests: " << passedTests << "\n";
        std::cout << "Failed tests: " << (totalTests - passedTests) << "\n";
    }
};

int main() {
    TestFramework framework;
    // 假设 .sy 文件和对应的 .out 文件存储在指定的目录中，例如 "tests/"
    std::string directoryPath = "../final_performance/";
//    std::string directoryPath = "../functional/";
//    std::string directoryPath = "../hidden_functional/";

    framework.runTestsInDirectory(directoryPath);

    return 0;
}