
bias = 0.1
results = [16]
i = 14
while i >= 0:
    level = results[-1] * 0.8
    results.append(level)
    i -= 1
results.reverse()
print(results)