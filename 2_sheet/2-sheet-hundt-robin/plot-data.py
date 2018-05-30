import matplotlib.pyplot as plt

# number of threads used to compute product of 2 matrices of dim. 1024
data_x = [1, 2, 3, 4, 8, 16, 32, 64, 128,
          256, 512, 1024, 2048, 4096]
# execution time in seconds
data_y = [3.300059, 1.664494, 2.294884, 3.200235, 
          2.915945, 3.082389, 3.023162, 3.012096,
          2.958028, 2.939918, 2.847527, 2.898556,
          2.876036, 2.963720]

plt.figure()
plt.plot(data_x, data_y)
plt.xlabel('# of threads')
plt.xscale('log')
plt.ylabel('execution time in seconds')
plt.title('Exection times of 1024x1024 matrix multi with different thread counts')
plt.show()