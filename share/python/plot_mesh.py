'''
    Small function to plot the mesh boundaries



'''
from matplotlib import pyplot as plt
from matplotlib.patches import Polygon
from matplotlib.collections import PatchCollection
import numpy as np
import sys

def read_meshdata(mesh_file):
    '''
    Function to read the raw mesh data
    '''
    with open(mesh_file, 'r') as reader:
        lines   = reader.readlines()

        # Filter out comments
        for i in range(len(lines)-1, -1, -1):
            identifier = lines[i].split(' ')[0]
            if (identifier == '>'):
                lines.pop(i)

        n_nodes = int(lines[0].split(' ')[1])

        nodes       = np.zeros( (n_nodes, 2) )
        boundaries  = {}
        front_edges = []
        tris        = []

        # Get node coordinates
        #----------------------------------------------------
        for i in range(1, n_nodes+1):
            line = lines[i].replace('\n','').split('\t')
            nodes[i-1][0] = float(line[1])
            nodes[i-1][1] = float(line[2])

        # Get boundary data
        #----------------------------------------------------
        lines = lines[n_nodes+1:]
        while (lines[0].split(' ')[0] == 'BOUNDARY'):

            boundary      = int(lines[0].split(' ')[1])
            n_bdry_edges  = int(lines[0].split(' ')[2])
            edges         = []
            boundaries.update({ boundary : edges })

            for i in range(1, n_bdry_edges+1):
                line = lines[i].replace('\n','').split('\t')
                edge = (int(line[1]), int(line[2]))
                edges.append(edge)

            lines = lines[n_bdry_edges+1:]

        # Get advancing front data
        #----------------------------------------------------
        n_front_edges = int(lines[0].split(' ')[1])

        for i in range(1, n_front_edges+1):
            line = lines[i].replace('\n','').split('\t')
            edge = (int(line[1]), int(line[2]))
            front_edges.append(edge)

        # Get triangle data
        #----------------------------------------------------
        lines  = lines[n_front_edges+1:]
        n_tris = int(lines[0].split(' ')[1])

        for i in range(1, n_tris+1):
            line = lines[i].replace('\n','').split('\t')
            tri = (int(line[1]), int(line[2]), int(line[3]))
            tris.append(tri)


    return nodes, boundaries, front_edges, tris


def main():
    ''' The main function '''
    if len(sys.argv) < 2:
        print("plot_boundary.py <mesh>.dat")
        sys.exit(1)

    nodes, boundaries, front_edges, tris = read_meshdata(sys.argv[1])


    fig, ax = plt.subplots(1,1,figsize=(10,10))

    for i_bdry, edges in boundaries.items():
        for e in edges:
            ax.plot(nodes[e,0], nodes[e,1], c='k',
                    lw=2.0, ls='-',marker='o')

    patches = {'edgecolors' : ['k'],
               'linewidths' : 0.5,
               'facecolors' : ['r'],
               'joinstyle'  : 'round',
               'capstyle'   : 'round',
               'alpha'      : 0.3,
               'cmap'       : 'coolwarm'}

    tri_patches = []
    for i_tri, tri in enumerate(tris):
        tri_patches.append(Polygon([nodes[tri[i]] for i in range(3)]))

    for e in front_edges:
        ax.plot(nodes[e,0], nodes[e,1], c='r',
                lw=3.0, ls='-',marker='o')

    for i, n in enumerate(nodes):
        ax.text(n[0],n[1],i)


    tri_col = PatchCollection(tri_patches, **patches)
    ax.add_collection(tri_col)

    ax.set_xlim((-2,12))
    ax.set_ylim((-2,12))

    plt.show()




if __name__ == '__main__': main()
