import matplotlib.pyplot as plt
import numpy as np

# Histogram example
# https://stackoverflow.com/a/33203848/2763915

# Scipy - for distributions
# https://stackoverflow.com/questions/51612755/heavy-tailed-and-normal-distribution-in-same-plot

# Number of possible contacts for a single agent
# Number of agents - 1
ntot = 10000

agent_int = []
with open('dead_interactions.txt', 'r') as fin:
	for line in fin:
		temp = line.strip().split(' ')
		agent_int.append(int(temp[-1]))

# Scale by total contacts possible
# agent_int = [x/ntot for x in agent_int]

# Histogram
plt.hist(agent_int, density=False, bins=10, label="Data", color = "red", histtype = "bar", ec = "black")
# Set x & y axis labels  
plt.xlabel("Number of Interactions with Dead", fontsize = 20) 
plt.ylabel("Number of Agents", fontsize = 20)
# Tick fontsize 
plt.xticks(fontsize = 15)
plt.yticks(fontsize = 15)

#mn, mx = plt.xlim()
#plt.xlim(mn, mx)
#kde_xs = np.linspace(mn, mx, 301)
#kde = st.gaussian_kde(x)
#plt.plot(kde_xs, kde.pdf(kde_xs), label="PDF")
#plt.legend(loc="upper left")
#plt.ylabel('Probability')
#plt.xlabel('Data')
plt.show()
