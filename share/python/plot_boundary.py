'''
    Small function to plot the mesh boundaries



'''
from matplotlib import pyplot as plt
import numpy as np
import sys

def read_meshdata(mesh_file):
    '''
    Function to read the raw mesh data
    '''
    with open(mesh_file, 'r') as reader:
        lines = reader.readlines()
        n_nodes = int(lines[0].split(' ')[1])

        nodes = np.zeros( (n_nodes, 2) )
        boundaries = {}

        for i in range(1, n_nodes+1):
            line = lines[i].replace('\n','').split('\t')
            nodes[i-1][0] = float(line[1])
            nodes[i-1][1] = float(line[2])

        lines = lines[n_nodes+1:]
        while (lines[0].split(' ')[0] == 'BOUNDARY'):

            boundary = int(lines[0].split(' ')[1])
            n_edges = int(lines[0].split(' ')[2])
            edges = []
            boundaries.update({ boundary : edges })

            for i in range(1, n_edges+1):
                line = lines[i].replace('\n','').split('\t')
                edge = (int(line[1]), int(line[2]))
                edges.append(edge)

            lines = lines[n_edges+1]

    return nodes, boundaries


def main():
    ''' The main function '''
    if len(sys.argv) < 2:
        print("plot_boundary.py <mesh>.dat")
        sys.exit(1)

    nodes, boundaries = read_meshdata(sys.argv[1])


    fig, ax = plt.subplots(1,1,figsize=(10,10))

    for i_bdry, edges in boundaries.items():
        for e in edges:
            ax.plot(nodes[e,0], nodes[e,1], c='k',
                    lw=2.0, ls='-',marker='o')


    plt.show()











if __name__ == '__main__': main()
