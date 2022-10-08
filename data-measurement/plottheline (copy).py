# Importing packages
import matplotlib.pyplot as plt

# Define x and y values
#x = [7, 14, 21, 28, 35, 42, 49]
#y = [8, 13, 21, 30, 31, 44, 50]

#x = [4, 16, 64, 256, 1024, 4096, 16384, 65536, 262144, 524288]
#x = [str(x) for x in x]

x = ['4', '16', '64', '256', '1Ki', '4Ki', '16Ki', '64Ki', '256Ki', '512Ki']

# this is for safi pipe latency:
#y = [75266.32964, 105276.781564, 123136.180894, 136954.088747, 326992.879752, 965803.912586, 3648503.755461, 14223008.368146, 32109704.801073, 47057698.649433]

# this is for safi pipe throughput:
#y = [24168.067722, 122767.332593, 292157.622048, 1018602.371156, 1831636.873393, 2382774.101145, 2548323.294995, 2625348.456891, 3394149.471708, 5728532.409833]

# this is for safi socket throughput
#y = [93378.524144, 195891.245572, 301429.507251, 1019104.400889, 2506735.409379, 4015172.801251, 4794470.097352, 9331802.456748, 10844875.858212, 15103190.343089]

# this is for safi socket latency:
y = [100836.132447, 111141.852391, 136394.548347, 257001.25119, 682953.847064, 2365889.985658, 9023745.447208, 22466946.855034, 38894210.551396, 61542150.485773]



# enable this for latency, disable for throughput:
y = [y / 1000.0 for y in y]

# Plot a simple line chart without any feature
plt.plot(x, y)
plt.xlabel("Message Size (bytes)")
plt.ylabel("Latency (us)")
#plt.title("Pipe Latency")
#plt.xlabel("Message Size (bytes)")
#plt.ylabel("Message Throughput (bytes / second)")
plt.title("Socket Latency")
plt.show()
