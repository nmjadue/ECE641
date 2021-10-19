import numpy as np
import matplotlib.pyplot as plt

h = open('cost_function_1_1.txt', 'r')

content = h.readlines()

values = []
difference = []
# powers = []
for line in content:
	line = line.replace('\n', '')
	values.append(float(line))
	# powers.append(0.1*len(values))


for i in range(1, len(values)):
	difference.append(values[i-1]-values[i])

np_val = np.array(values)
# pow = np.array(powers)
print(difference)

fig, ax = plt.subplots()
plt.plot(np_val)
plt.title("Cost Function")
plt.xlabel("Iteration")
plt.ylabel("Cost")
plt.show()

fig, ax = plt.subplots()
plt.plot(difference)
plt.plot(np.zeros(len(difference)), 'k')
plt.title("Difference Plot")
plt.ylabel("Difference between iteration i+1 and i")
plt.xlabel("Iteration")
plt.show()