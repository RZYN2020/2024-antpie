import os
import subprocess
import sys

lib_dir = "tests/"
test_dir = "tests/compiler2023/公开样例与运行时库/functional/"
compiler_path = "build/compiler"
tmp_file_base = os.path.abspath(".") + "/tmp/"

def find_and_sort_matching_files(directory):
    # 获取文件夹中的所有文件名
    all_files = os.listdir(directory)
    
    # 创建字典以存储文件前缀和对应的文件
    files_dict = {}

    for file in all_files:
        # 获取文件的前缀和后缀
        prefix, suffix = os.path.splitext(file)
        if suffix in ['.sy', '.out', '.in']:
            if prefix not in files_dict:
                files_dict[prefix] = {}
            files_dict[prefix][suffix] = file

    # 找到匹配的文件对，并排序
    matched_files = []
    for prefix, suffixes in files_dict.items():
        if '.sy' in suffixes and '.out' in suffixes:
            matched_files.append((suffixes['.sy'], suffixes['.out'], suffixes.get('.in')))

    # 按前缀排序
    matched_files.sort(key=lambda x: os.path.splitext(x[0])[0])

    return matched_files

def compare_file_to_string(file_path, actual_output):
    flag = True
    try:
        with open(file_path, 'r') as file:
            # 将实际输出按行拆分
            actual_lines = actual_output.splitlines()
            # 逐行读取文件并比较
            for i, expected_line in enumerate(file):
                expected_line = expected_line.rstrip('\n')  # 移除行尾的换行符
                if i >= len(actual_lines):
                    print(f"Line {i + 1}: File has more lines than actual output.")
                    print(f"Expected: {expected_line}")
                    flag = False
                    break
                actual_line = actual_lines[i]
                if expected_line != actual_line:
                    flag = False
                    print(f"Line {i + 1}:")
                    print(f"Expected: {expected_line}")
                    print(f"Actual:   {actual_line}")
            # 检查实际输出是否有多余的行
            if len(actual_lines) > i + 1:
                for j in range(i + 1, len(actual_lines)):
                    flag = False
                    print(f"Line {j + 1}: Actual output has more lines than file.")
                    print(f"Actual:   {actual_lines[j]}")
                    
    except FileNotFoundError:
        print(f"File not found: {file_path}")
    except Exception as e:
        print(f"An error occurred: {e}")
    return flag

def run_single_ir_test(sy_file, out_file, in_file=None):
    # generate ir command: compiler ...00_main.sy -o tmp/00_main.ll -l
    prefix = sy_file.split('.')[0] 
    # get llvm ir file
    ir_file = tmp_file_base + prefix + ".ll"
    gen_command = compiler_path + " " + test_dir + sy_file + " -o " + ir_file + " -l"
    subprocess.run(gen_command, shell=True)

    # llvm ir to bitcode
    # llvm-as example.ll -o example.bc
    bc_file = tmp_file_base + prefix + ".bc"
    bcgen_command = ["llvm-as", ir_file, "-o", bc_file]
    result = subprocess.run(bcgen_command, text=True, capture_output=True)

    # bitcode to x64-64 code
    # llc example.bc -o example.s
    code_file = tmp_file_base + prefix + ".s"
    codegen_command = ["llc", "-relocation-model=pic", bc_file, "-o", code_file]
    result = subprocess.run(codegen_command, text=True, capture_output=True)

    # asm to object file
    # clang -c example.s -o example.o
    obj_file = tmp_file_base + prefix + ".o"
    objgen_command = ["clang", "-fPIE", "-c", code_file, "-o", obj_file]
    result = subprocess.run(objgen_command, text=True, capture_output=True)

    # link
    # clang example.o -L. -lsysy -o example
    bin_file = tmp_file_base + prefix 
    link_command = ["clang", obj_file, "-L", lib_dir, "-lsysy", "-o", bin_file]
    result = subprocess.run(link_command, text=True, capture_output=True)

    # run ir command: lli
    run_command = [bin_file]
    if in_file:
        # 如果有输入文件，将其作为标准输入
        with open(test_dir + in_file, 'r') as input_file:
            result = subprocess.run(run_command, text=True, input=input_file.read(), capture_output=True)
    else:
        result = subprocess.run(run_command, text=True, capture_output=True)
    stdout = result.stdout
    if stdout != "" and stdout[len(stdout) - 1] != '\n':
        result.stdout += "\n"
    output = result.stdout + str(result.returncode)
    return compare_file_to_string(test_dir + out_file, output)

def print_result(success, fail):
    if fail == 0:
        print('[Success]' + ' 🎉')
    else:
        print('[Failure]' + ' 💩')
    print(str(success) + '/' + str(success + fail))

def run_all_ir_test(sorted_files):
    success = 0
    fail = 0
    for sy_file, out_file, in_file in sorted_files:
        print("Running test: " + sy_file)
        if run_single_ir_test(sy_file, out_file, in_file):
            success += 1
        else:
            fail += 1
    print_result(success, fail)

if __name__ == "__main__":
    mode = sys.argv[1]
    sorted_files = find_and_sort_matching_files(test_dir)
    if mode == 'RISCV':
        # TODO
        print("unfinish")
        exit(0)
    if mode == 'LLVM':
        run_all_ir_test(sorted_files)
