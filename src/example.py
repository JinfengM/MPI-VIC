import subprocess

# 调用C可执行程序
process = subprocess.Popen(['mpirun', '-np', '24', './MPI-VIC.exe', '-g', 'chanliu_input.txt', '0.910654', '0.436423', '24.482024', '0.502105', '0.988413', '0.413380'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

# 获取标准输出和标准错误
stdout, stderr = process.communicate()

# 显示标准输出内容
if stdout:
    output_lines = stdout.decode().strip().split('\n')  # 将输出按行分割
    last_line = output_lines[-1]  # 获取最后一行
    result = last_line #得到输出结果目录
    import numpy as np
    # 读取观测值和模拟值
    observed_data = np.genfromtxt(result+'luanx.csv', delimiter=',')
    simulated_data = np.genfromtxt(result+'luanx.month', usecols=(2))

    # 计算纳什效率系数NSE
    NSE = 1 - np.sum((observed_data - simulated_data)**2) / np.sum((observed_data - np.mean(observed_data))**2)
    print(f"纳什效率系数NSE: {NSE}")
else:
    print(f"错误: {stderr.decode().strip()}")