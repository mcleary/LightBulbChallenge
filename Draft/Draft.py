import numpy as np
import matplotlib.pyplot as plt
import csv


input_filepath = r'D:\Dropbox\Documentos\Curriculos\Thalmic Labs\Vanhackaton Challenge\challenge data\ciexyz31.csv'

cie_lambda = []
cie_x = []
cie_y = []
cie_z = []

with open(input_filepath, 'r') as csvfile:
    reader = csv.reader(csvfile, delimiter=',')
    for row in reader:
        cie_lambda += [float(row[0])]
        cie_x += [float(row[1])]
        cie_y += [float(row[2])]
        cie_z += [float(row[3])]

cie_lambda = np.array(cie_lambda)
cie_x = np.array(cie_x)
cie_y = np.array(cie_y)
cie_z = np.array(cie_z)

print(len(cie_lambda))

plt.figure()
plt.plot(cie_lambda, cie_x)
plt.plot(cie_lambda, cie_y)
plt.plot(cie_lambda, cie_z)
plt.show()




