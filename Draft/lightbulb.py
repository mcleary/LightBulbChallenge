import numpy as np
import matplotlib.pyplot as plt
import csv
import math

wavelengths_filepath = r'D:\Dropbox\Documentos\Curriculos\Thalmic Labs\Vanhackaton Challenge\challenge data\c\wavelengths.csv'
intensities_filepath = r'D:\Dropbox\Documentos\Curriculos\Thalmic Labs\Vanhackaton Challenge\challenge data\c\intensities.csv'
colormatching_filepath = r'D:\Dropbox\Documentos\Curriculos\Thalmic Labs\Vanhackaton Challenge\challenge data\ciexyz31.csv'


def read_csv(filepath):        
    file_contents = []
    with open(filepath, 'r') as csvfile:
        reader = csv.reader(csvfile, delimiter=',')
        for row in reader:
            if row[0].startswith('#'):
                continue
            file_contents.append(row)
    return np.array(file_contents, dtype=np.float64)


def moving_average(a, n=10) :
    result = []    
    for i in range(len(a)):
        sum = 0
        count = 0        
        for j in range(i - math.floor(n/2), i + math.floor(n/2)):
            if 0 <= j < len(a):
                sum += a[j]
                count += 1
        result += [sum / count]
    return np.array(result)


def lerp(alpha, a, b):
        return a + alpha * (b - a)


def average_spectrum_samples(l, vals, n, lambda_start, lambda_end):
    if lambda_end <= l[0]:
        return vals[0]
    if lambda_start >= l[n-1]:
        return vals[n-1]
    if n == 1:
        return vals[0]

    sum = 0.0

    if lambda_start < l[0]:
        sum += vals[0] * (l[0] - lambda_start)
    if lambda_end > l[n-1]:
        sum += vals[n-1] * (lambda_end - l[n-1])

    i = 0
    while lambda_start > l[i+1]:
        i += 1    
            
    def interp(w, i):
        return lerp((w - l[i]) / (l[i+1] - l[i]), vals[i], vals[i+1])

    def seg_avg(wl0, wl1, i):
        return 0.5 * (interp(wl0, i) + interp(wl1, i))

    while i+1 < n and lambda_end >= l[i]:        
        seg_start = max(lambda_start, l[i])
        seg_end = min(lambda_end, l[i+1])
        sum += seg_avg(seg_start, seg_end, i) * (seg_end - seg_start)
        i += 1

    return sum / (lambda_end - lambda_start)


def main():
    wavelengths = read_csv(wavelengths_filepath)
    intensites = read_csv(intensities_filepath)
    color_matching = read_csv(colormatching_filepath)       

    # Remove Noise
    for i in range(len(intensites)):
        intensites[i] = moving_average(intensites[i])
        
    # Normalize Color Matching function
    for i in range(1, 4):
        a = color_matching[:,i]
        cmin = a.min()
        cmax = a.max()
        color_matching[:,i] = (color_matching[:,i] - cmin) / (cmax - cmin)

    cie_lambda = color_matching[:,0]
    cie_x = color_matching[:,1]
    cie_y = color_matching[:,2]
    cie_z = color_matching[:,3]
    n_cie_samples = len(cie_lambda)

    sampled_lambda_start = 340
    sampled_lambda_end = 840
    n_spectral_samples = 10

    plt.figure()
        
    for sample_idx in range(len(intensites)):
    #for sample_idx in range(20):
        print("Sample " + str(sample_idx) + " of " + str(len(intensites)))
        sample_data = intensites[sample_idx]
        n_spectral_samples = len(sample_data)

        x_c = []
        y_c = []
        z_c = []

        sample_data_c = []

        for i in range(n_spectral_samples):
            wl0 = lerp(float(i) / float(n_spectral_samples), sampled_lambda_start, sampled_lambda_end)
            wl1 = lerp(float(i+1) / float(n_spectral_samples), sampled_lambda_start, sampled_lambda_end)
            x_c += [average_spectrum_samples(cie_lambda, cie_x, n_cie_samples, wl0, wl1)]
            y_c += [average_spectrum_samples(cie_lambda, cie_y, n_cie_samples, wl0, wl1)]
            z_c += [average_spectrum_samples(cie_lambda, cie_z, n_cie_samples, wl0, wl1)]    

            sample_data_c += [average_spectrum_samples(wavelengths[:,1], sample_data, n_spectral_samples, wl0, wl1)]

        x_sum = 0.0
        y_sum = 0.0
        z_sum = 0.0        
        for i in range(n_spectral_samples):
            x_sum += x_c[i] * sample_data_c[i]
            y_sum += y_c[i] * sample_data_c[i]
            z_sum += z_c[i] * sample_data_c[i]

        #yint = 0.0        
        #for i in range(n_spectral_samples):
        #    yint += y_c[i]

        cie_xyz_x = x_sum / (x_sum + y_sum + z_sum)
        cie_xyz_y = y_sum / (x_sum + y_sum + z_sum)          
        
        print(cie_xyz_x, cie_xyz_y)

        plt.scatter(cie_xyz_x, cie_xyz_y)     
            
        #plt.plot(wavelengths[:,1], x_c)
        #plt.plot(wavelengths[:,1], x1_c)
        
        # plt.plot(wavelengths[:,1], z_c)
    
    #for i in range(1, 4):
    #    plt.plot(color_matching[:,0], color_matching[:,i])

    #for i in range(1):        
    #    plt.plot(wavelengths[:,1], intensites[i])    
    plt.xlim([0, 0.7])
    plt.ylim([0, 0.8])
    plt.show()
    

if __name__ == '__main__':
    main()
