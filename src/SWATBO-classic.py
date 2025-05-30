from bayes_opt import BayesianOptimization
from bayes_opt import acquisition
import subprocess
import time
# 定义目标函数
def black_box_function(x1, x2,x3,x4,x5,x6):

    #调用vic模型
    process = subprocess.Popen(['mpirun', '-np', '24', './MPI-VIC.exe', '-g', 'chanliu_input.txt', str(x1), str(x2), str(x3), str(x4), str(x5), str(x6)], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    # 获取标准输出和标准错误
    stdout, stderr = process.communicate()
    # 显示标准输出内容
    if stdout:
        output_lines = stdout.decode().strip().split('\n')  # 将输出按行分割
        last_line = output_lines[-1]  # 获取最后一行
        result = last_line #得到输出结果目录
        import numpy as np
        # 读取观测值和模拟值:如果是月，则读取luanx.month，如果是日，则读取luanx.day
        #月尺度时使用rout_input.txt中的第二行起止时间
        #日尺度时使用rout_input.txt中的第一行起止时间
        observed_data = np.genfromtxt(result+'luanx.csv', delimiter=',')
        #simulated_data = np.genfromtxt(result+'luanx.month', usecols=(2))
        #日输出使用第四列usecols=(3)，月输出使用第三列usecols=(2)
        simulated_data = np.genfromtxt(result+'luanx.day', usecols=(3))
        simulated_data=simulated_data[-len(observed_data):]       
        # 计算纳什效率系数NSE
        NSE = 1 - np.sum((observed_data - simulated_data)**2) / np.sum((observed_data - np.mean(observed_data))**2)
                # 将参数和模拟值写入外部文件
        with open('simulation_parameters_results.txt', 'a') as f:
            f.write(f"{x1},{x2},{x3},{x4},{x5},{x6},")
            for value in simulated_data:
                f.write(f"{value},")
            f.write(f"{NSE}\n")
        #print(f"纳什效率系数NSE: {NSE}")
    else:
        print(f"错误: {stderr.decode().strip()}")
        NSE = 0

    return NSE
#ucb = acquisition.UpperConfidenceBound(kappa=10)
#ei = acquisition.ExpectedImprovement(xi=0.1)
#BORG_Problem_set_bounds(problem, 0, 0.01, 0.5);//b_infilt :(0-1)
#BORG_Problem_set_bounds(problem, 1, 0.01, 1);//Ds       :(0.001-1)
#BORG_Problem_set_bounds(problem, 2, 0.1, 30);//Dsmax       :(0.1-50)
#BORG_Problem_set_bounds(problem, 3, 0.01, 1.0);//Ws       :(0-1.0)
#BORG_Problem_set_bounds(problem, 4, 0.1, 1.5);//depth[1]   :(0.1-3.0)
#BORG_Problem_set_bounds(problem, 5, 0.1, 1.5);//depth[2]   :(0.1-3.0)
pi = acquisition.ProbabilityOfImprovement(xi=0.1)
# 创建优化器
optimizer = BayesianOptimization(
    acquisition_function=pi,
    f=None,
    pbounds={'x1': (0.01, 0.5),'x2': (0.01, 1),'x3': (0.1, 50),'x4': (0.01, 1.0),'x5': (0.1, 1.5),'x6': (0.1, 1.5)},
    verbose=2,
    random_state=1,
)

# 使用suggest-evaluate-register模式进行优化
n_iter = 500  # 总迭代次数


# 在优化过程中可以切换使用不同的采集函数
for i in range(n_iter):
    start_time = time.time()
    print(f"\n###VIC模型参数自动率定### 第 {i+1} 次迭代:")
   # 1. Suggest: 获取下一个建议的采样点
    next_point = optimizer.suggest()
    print("建议采样点:", next_point)
    
    # 2. Evaluate: 评估目标函数
    target = black_box_function(**next_point)
    print("目标函数值:", target)
    
    # 3. Register: 注册结果
    optimizer.register(
        params=next_point,
        target=target
    )
    end_time = time.time()
    print(f"迭代时间: {end_time - start_time:.2f} 秒")  

# 打印最优结果
print("\n最优解:")
print(f"x1 = {optimizer.max['params']['x1']:.4f}")
print(f"x2 = {optimizer.max['params']['x2']:.4f}")
print(f"x3 = {optimizer.max['params']['x3']:.4f}")
print(f"x4 = {optimizer.max['params']['x4']:.4f}")
print(f"x5 = {optimizer.max['params']['x5']:.4f}")
print(f"x6 = {optimizer.max['params']['x6']:.4f}")
print(f"目标函数最大值 = {optimizer.max['target']:.4f}")