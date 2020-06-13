'''
    Small function to plot a quadtree structure



'''

from matplotlib.ticker import MaxNLocator
from matplotlib.colors import BoundaryNorm
from mpl_toolkits.axes_grid1 import make_axes_locatable

from matplotlib import pyplot as plt
from matplotlib.patches import Rectangle
from matplotlib.collections import PatchCollection
import numpy as np
import sys, os

QUAD_PATCH = {'edgecolors' : ['k'],
              'linewidths' : 0.5,
              'facecolors' : ['None'],
              'joinstyle'  : 'round',
              'capstyle'   : 'round',
              'alpha'      : 1.0,
              'cmap'       : 'Greys'}

def read_quaddata(mesh_file):
    '''
    Function to read the raw quadtree data
    '''
    with open(mesh_file, 'r') as reader:
        lines   = reader.readlines()

        # Filter out comments
        for i in range(len(lines)-1, -1, -1):
            identifier = lines[i].split(' ')[0]
            if (identifier == '>'):
                lines.pop(i)

        quads      = []
        layers     = []

        # Get quad coordinates
        #----------------------------------------------------
        for line in lines:
            cur_line = line.replace('\n','').split('\t')
            layer = int(cur_line[0])
            x_min = float(cur_line[1])
            y_min = float(cur_line[2])
            x_max = float(cur_line[3])
            y_max = float(cur_line[4])
            quads.append( [(x_min, y_min), (x_max, y_max)] )
            layers.append(layer)

    return np.array(quads), np.array(layers)

def main():
    ''' The main function '''
    if len(sys.argv) < 3:
        print("plot_qtree.py <qtree>.dat <export_path>")
        sys.exit(1)

    quads, layers = read_quaddata(sys.argv[1])
    export_path = sys.argv[2]

    fig, ax = plt.subplots(1,1,figsize=(24,18))
    ax.set_aspect('equal')

    quad_patches = []
    for i, q in enumerate(quads):
        width  = q[1][0] - q[0][0]
        height = q[1][1] - q[0][1]
        quad_patches.append(Rectangle(q[0], width, height))

    '''
    # Add colormap for layers
    # ---------------------------------
    cmap = plt.get_cmap(QUAD_PATCH['cmap'])
    levels = MaxNLocator(nbins=25).tick_values(layers.min(),
                                               layers.max())
    norm = BoundaryNorm(levels, ncolors=cmap.N, clip=True)
    QUAD_PATCH.update({'norm':norm})
    '''

    quad_col = PatchCollection(quad_patches, **QUAD_PATCH)
    # quad_col.set_array(layers)
    ax.add_collection(quad_col)

    ax.set_xlim(( quads[:,0][:,0].min(), quads[:,1][:,0].max()) )
    ax.set_ylim(( quads[:,0][:,1].min(), quads[:,1][:,1].max()) )
    ax.set_axis_off()

    fig_path = os.path.join( export_path, 'qtree')
    print("Exporting {:}".format(fig_path))
    plt.tight_layout()
    fig.savefig(fig_path + '.png', dpi=300)
    plt.close(fig)
    print("Done")

if __name__ == '__main__': main()
