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
    # Performance with version 0.2
    N1 = np.array([29, 106, 464, 1940, 11396, 45842, 184070, 1055218, 4221754])
    T1 = np.array([7.22e-04, 2.88e-03, 6.56e-03, 3.44e-02, 1.48e-01, 5.76e-01, 2.90e+00, 2.03e+01, 9.13e+01])

    # Performance with version 0.3 ( with edge size functions)
    N2 = np.array([4408,13048,326245, 1307193])
    T2 = np.array([2.49e-01,6.12e-01,9.88e+00,1.71e+02])
    # Performance with version 0.3 ( without edge size functions)
    N3 = np.array([3149,12698,672676,2096774])
    T3 = np.array([1.68e-01,5.22e-01,2.23e+01,6.37e+01])

    log_1 = log_line(N1, 5e-2, 2e-6, 1)
    #log_2 = log_line(N, 5e-2, 1e-9, 2)

    fig, ax = plt.subplots(1,1,figsize=(5,3))
    #ax.plot(N0, T0,    c=(.5,.5,.9), ls='-',  marker='o', lw=2.0, label='tMesh - v0.1')
    ax.plot(N1,  T1,     c=(.3,.3,.9), ls='-',  marker='o', lw=2.0, label='tMesh - v0.2')

    ax.plot(N2,  T2,     c=(.9,.3,.3), ls='-',  marker='o', lw=2.0, label='tMesh - v0.3a')
    ax.plot(N3,  T3,     c=(.9,.3,.3), ls='--',  marker='o', lw=2.0, label='tMesh - v0.3b')

    ax.plot(N1,  log_1, c=(.1,.1,.1), ls='--', label='$\mathcal{O}(N)$')
    #ax.plot(N,  log_2, c=(.5,.5,.5), ls='--', label='$\mathcal{O}(N^2)$')

    ax.legend()

    ax.set_yscale('log')
    ax.set_xscale('log')

    ax.set_xlabel('Number of triangles')
    ax.set_ylabel('Meshing time in seconds')

    #ax.grid(True)

    fig_path = os.path.join( export_path, 'performance')
    print("Exporting {:}".format(fig_path))
    plt.tight_layout()
    fig.savefig(fig_path + '.png', dpi=280)
    plt.close(fig)
    print("Done")


if __name__ == '__main__': main()
