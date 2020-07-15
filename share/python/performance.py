from matplotlib import pyplot as plt
import numpy as np
import sys, os



def main():
    if len(sys.argv) < 2:
        print("performanc.py <export_directory>")
        sys.exit(1)

    export_path = sys.argv[1]

    log_line = lambda x, Px, Py, n: (x**n) * np.exp(np.log(Py) - n * np.log(Px))

    # Performance with old version (bubble sort & other things)
    N0 = np.array([46, 534, 2164, 8592])
    T0 = np.array([2.25e-03, 1.12e-01, 2.01e+00, 4.14e+01])

    # Performance with new version
    N  = np.array([29, 106, 464, 1940, 11396, 45842, 184070, 1055218, 4221754])
    T  = np.array([7.22e-04, 2.88e-03, 6.56e-03, 3.44e-02, 1.48e-01, 5.76e-01, 2.90e+00, 2.03e+01, 9.13e+01])

    log_1 = log_line(N, 5e-2, 2e-6, 1)
    #log_2 = log_line(N, 5e-2, 1e-9, 2)

    fig, ax = plt.subplots(1,1,figsize=(5,3))
    #ax.plot(N0, T0,    c=(.5,.5,.9), ls='-',  marker='o', lw=2.0, label='tMesh - v0.1')
    ax.plot(N,  T,     c=(.3,.3,.9), ls='-',  marker='o', lw=2.0, label='tMesh - v0.2')
    ax.plot(N,  log_1, c=(.1,.1,.1), ls='--', label='$\mathcal{O}(N)$')
    #ax.plot(N,  log_2, c=(.5,.5,.5), ls='--', label='$\mathcal{O}(N^2)$')

    ax.legend()

    ax.set_yscale('log')
    ax.set_xscale('log')

    #ax.grid(True)

    fig_path = os.path.join( export_path, 'performance')
    print("Exporting {:}".format(fig_path))
    plt.tight_layout()
    fig.savefig(fig_path + '.png', dpi=280)
    plt.close(fig)
    print("Done")


if __name__ == '__main__': main()
