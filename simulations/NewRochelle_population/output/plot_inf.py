import numpy as np
import matplotlib.pyplot as plt 
import matplotlib

# Number of steps
Nt = 400
# Time step
dt = 0.25
ntot = 79205
t = np.linspace(0, Nt*dt, num=Nt+1)

# Load 
infected = np.loadtxt('infected_with_time.txt')
infected1 = [(x/ntot) * 100 for x in infected]
exposed = np.loadtxt('exposed_with_time.txt')
exposed1 = [(x/ntot)*100 for x in exposed]
susceptible = np.loadtxt("susceptible_with_time.txt")
susceptible1 = [(x/ntot)*100 for x in susceptible]
removed = np.loadtxt("removed_with_time.txt")
removed1 = [(x/ntot)*100 for x in removed]

fig, ax = plt.subplots()

ax.plot(t,infected1, "r", linewidth=2.5, label="Infected")
ax.plot(t,exposed1, "m", linewidth=2.5, label="Exposed")
ax.plot(t,susceptible1, "y", linewidth=2.5, label="Susceptible")
ax.plot(t,removed1, "b", linewidth=2.5, label="Removed")

plt.xlabel("Time (days)", fontsize=22)
plt.ylabel("Percent of Population", fontsize=22)

plt.xticks(fontsize=18)
plt.yticks(fontsize=18)

plt.xlim([0, 100])
plt.ylim([0, 100])

plt.grid()
plt.legend(fontsize=23)
plt.show()
