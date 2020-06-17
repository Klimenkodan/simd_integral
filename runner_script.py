import sys
import subprocess
NUM_OF_OUTPUT_VALUES = 6

# Крім того, потрібно написати скрипт (bash, Python), який запустить програму вказану кількість раз та:

# - Порівняє, чи результати всіх її запусків співпадають (в межах похибки -- порівнювати виведений текст недостатньо).

# - Виведе найменший час роботи за виконані запуски.


def parser(output):
    """
    The function, which parse the result of the program execution and return it as dictionary
    output : str
    """
    output_values = dict()
    lst_output = output.split('\n')
    
    for i in (lst_output):
        ind = 0
        str_key = ''
        str_num = ''
        while ind < len(i) and i[ind] != ':' :
            str_key += i[ind]
            ind += 1
        ind += 2
        while ind < len(i) and i[ind] != 'e':
            str_num += i[ind]
            ind += 1
        output_values[str_key] = float(str_num)
    return output_values

def run(times, config_file):
    '''
    The main function which run the program times number of times.\ 
    It checks, whether results of executions were similar and returns the time of the fastest execution
    '''
    
    
    subprocess.call("cmake .", shell=True)
    subprocess.call("make", shell=True)

    result = subprocess.check_output(f"./sampleint {config_file}", shell=True).decode('utf-8')
    output = parser(result)
    min_result = output['Time'] / (10 ** 6)

    for _ in range(times - 1):
        cur_result = subprocess.check_output(f"./sampleint {config_file}", shell=True).decode('utf-8')
        cur_output = parser(cur_result)
        if abs(output['Result'] - cur_output['Result']) / output['Result'] <= output["Rel err"]:
            min_result = min(min_result, cur_output["Time"] / 10 ** 6)

    
    return min_result


if __name__ == "__main__":
    cla = sys.argv
    dbg = run(cla[1], cla[2])
    print(dbg)
    