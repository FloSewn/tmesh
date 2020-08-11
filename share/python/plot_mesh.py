'''
    Small function to plot the mesh boundaries



'''
from matplotlib import pyplot as plt
from matplotlib.patches import Polygon
from matplotlib.collections import PatchCollection
import numpy as np
import sys, os

TRI_PATCH = {'edgecolors' : ['k'],
             'linewidths' : 0.9,
             'facecolors' : ['w'],
             'joinstyle'  : 'round',
             'capstyle'   : 'round',
             'alpha'      : 1.0,
             'cmap'       : 'coolwarm'}

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
        bdry_marker = {}
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
                edge   = (int(line[1]), int(line[2]))
                marker =  int(line[3])
                edges.append(edge)

                if (marker in bdry_marker.keys()):
                    bdry_marker[marker].append(edge)
                else:
                    bdry_marker.update({marker : [edge]})

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


    return nodes, boundaries, bdry_marker, front_edges, tris


def main():
    ''' The main function '''
    if len(sys.argv) < 3:
        print("plot_boundary.py <mesh>.dat <export_path>")
        sys.exit(1)

    nodes, boundaries, bdry_marker, front_edges, tris = read_meshdata(sys.argv[1])
    export_path = sys.argv[2]

    i_plts = 0

    for step in range(len(tris), len(tris)-1-i_plts, -1):

        fig, ax = plt.subplots(1,1,figsize=(8,4))
        ax.set_aspect('equal')

        #for i_bdry, edges in boundaries.items():
        #    for e in edges:
        #        ax.plot(nodes[e,0], nodes[e,1], c='k',
        #                lw=2.0, ls='-',marker='o')

        #for i, n in enumerate(nodes):
        #    ax.text(n[0],n[1],i, color='b')

        if step == len(tris):
            for e in front_edges:
                ax.plot(nodes[e,0], nodes[e,1], c='r',
                        lw=1.0, ls='-') #,marker='o')

        tri_patches = []
        for i_tri, tri in enumerate(tris[:step]):
            tri_patches.append(Polygon([nodes[tri[i]] for i in range(3)]))
            #tri_centr = np.mean([nodes[tri[i]] for i in range(3)], axis=0)
            #ax.text(tri_centr[0], tri_centr[1], i_tri, color='k')

        tri_col = PatchCollection(tri_patches, **TRI_PATCH)
        ax.add_collection(tri_col)

        ax.set_xlim((nodes[:,0].min(),nodes[:,0].max()))
        ax.set_ylim((nodes[:,1].min(),nodes[:,1].max()))
        ax.set_axis_off()

        fig_path = os.path.join( export_path, 'mesh_step_{:}'.format(len(tris)-step))
        print("Exporting {:}".format(fig_path))
        plt.tight_layout()
        fig.savefig(fig_path + '.png', dpi=280, transparent=True)
        plt.close(fig)
        print("Done")




if __name__ == '__main__': main()
