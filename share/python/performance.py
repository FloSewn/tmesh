from matplotlib import pyplot as plt
import numpy as np


log_line = lambda x, Px, Py, n: (x**n) * np.exp(np.log(Py) - n * np.log(Px))


N = np.array([46, 534, 2164, 8592])
T = np.array([2.25e-03, 1.12e-01, 2.01e+00, 4.14e+01])


log_1 = log_line(N, 5e-2, 1e-5, 1)
log_2 = log_line(N, 5e-2, 1e-9, 2)
log_3 = log_line(N, 5e-2, 1e-13, 3)

fig, ax = plt.subplots(1,1,figsize=(10,10))
ax.plot(N, T,     c=(.3,.3,.9), ls='-',  lw=2.0, label='trimesh')
ax.plot(N, log_1, c=(.1,.1,.1), ls='--', label='$\mathcal{O}(N)$')
ax.plot(N, log_2, c=(.5,.5,.5), ls='--', label='$\mathcal{O}(N^2)$')
ax.plot(N, log_3, c=(.7,.7,.7), ls='--', label='$\mathcal{O}(N^3)$')

ax.legend()

ax.set_yscale('log')
ax.set_xscale('log')



ax.grid(True)
plt.show()
