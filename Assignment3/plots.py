#!/usr/bin/env python
# coding: utf-8

# In[57]:


import pandas as pd
import numpy as np
import matplotlib.pyplot as plt


# In[58]:


def plot_data(name,legend) :
    df = pd.read_csv(name, sep="\s+",header=None)
    plt.plot(df[0],df[1],label=legend)
    plt.legend(loc='best')


# In[59]:


names = ["Hybla","NewReno","Scalable","Vegas","Westwood"]
plt.rcParams['figure.figsize'] = (20,10)


# In[60]:


plt.xlim(0.0,1.80)
plt.xlabel("Time(s)")
plt.ylabel("Cumulative Bytes received")
plt.title("Cumulative Bytes received vs. Time(s)")
for name in names:
    plot_data("bytes_Tcp"+name+".dat",name)
plt.grid()
plt.savefig('cum_bytes.jpg')
plt.show()


# In[61]:


plt.xlim(0.0,1.80)
plt.xlabel("Time(s)")
plt.ylabel("Window Size(B)")
plt.title("Window Size(B) vs. Time(s)")
for name in names:
    plot_data("cw_Tcp"+name+".dat",name)
plt.grid()
plt.savefig('win_size.jpg')
plt.show()


# In[62]:


plt.xlim(0.0,1.80)
plt.xlabel("Time(s)")
plt.ylabel("Number of packets dropped")
plt.title("Number of packets dropped vs. Time(s)")
for name in names:
    plot_data("drop_Tcp"+name+".dat",name)
plt.grid()
plt.savefig('packets_drop.jpg')
plt.show()

