import math

def test(filename, func, tolerance = 1e-3):
    with open(filename, 'r') as file:
        for line in file:
            try:
                expr, result_str = line.split('=')
                expr = expr.strip()
                result_str = result_str.strip()

                if '(' in expr and ')' in expr:
                    arg = float(expr.split('(')[1].split(')')[0])
                else:
                    arg = float(expr.split('^')[0])

                expected = func(arg)

                actual = float(result_str)

                if not math.isclose(actual, expected, rel_tol=tolerance):
                    print(f"Error in file {filename}: {line.strip()}")
                    print(f"We excpect: {expected}, and get: {actual}")
            except Exception as e:
                print(f"Error for string processing {filename}: {line.strip()} ({e})")

test("sin_results.txt", math.sin)
test("sqrt_results.txt", math.sqrt)
test("pow_results.txt", lambda x: x**2)

print("Finished")