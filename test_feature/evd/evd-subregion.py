#!/usr/bin/env python
from __future__ import print_function, division
import sys, os

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib.ticker as ticker

import ROOT
from ROOT import TFile

def plot(filename, option, x1=0, y1=0, y_rebin=4):
    print('opening ' + filename)
    f = TFile(filename)
    dirname = 'plots'

    if not os.path.exists(dirname):
        os.makedirs(dirname)

    ht1 = f.Get("%s" % option)
    nBinsX = int(ht1.GetNbinsX())
    nBinsY = int(ht1.GetNbinsY())
    # bin_start = 0
    print("nbinx= %d, nbiny= %d"%(nBinsX, nBinsY))

    # if (plane == 'u'):
    #     # nBinsX = 800
    #     bin_start = 490
    # elif (plane == 'v'):
    #     # nBinsX = 800
    #     bin_start = 800
    # elif (plane == 'w'):
    #     # nBinsX = 960
    #     bin_start = 2250

    scale_factor = 1.0
    if option=="decon": scale_factor = 10.0
    zt1 = np.zeros((nBinsX, nBinsY))
    for i in range(nBinsX):
        for j in range(nBinsY):
            zt1[i, j] =  ht1.GetBinContent(i+1, j+1) / scale_factor

    # colormap, ref: https://matplotlib.org/examples/color/colormaps_reference.html
    cmapname = 'seismic'
    # cmapname = 'rainbow'

    # cmapname = 'coolwarm'
    # cmapname = 'bwr'
    # cmapname = 'winter'
    # cmapname = 'cool'

    vrange = 300
    vmin = -vrange
    vmax = vrange

    if option=="decon":
        cmapname = 'rainbow'
        vmin = -300
        vmax = 2800

    # vrange = 300
    # vmin = -vrange
    # vmax = vrange
    # fontsize = 24*1.5
    fontsize = 30
    labelsize= 30
    dpi = 80
    cmap = plt.get_cmap(cmapname)
    #if plane == 'w': plane = 'x'

    # print("figsize=(%.3f, %.3f)"%(nBinsX*4.71/dpi, nBinsY*0.5*1.6/dpi))
    figsizeY = 10
    figsizeX = figsizeY*(nBinsX*4.71)/(nBinsY*y_rebin*0.5*1.6) # 4.71mm/wire, 0.5us/tick, drift 1.6mm/us
    fig, ax = plt.subplots(1,1,figsize=(figsizeX,figsizeY), dpi=dpi) # also see aspect ratio
    # fig, ax = plt.subplots(1,1)
    yticks = ticker.FuncFormatter(lambda x, pos: '{0:g}'.format( 0.5*(x*y_rebin + y1) )) # x: bin number, pos: label value
    ax.yaxis.set_major_formatter(yticks)
    xticks = ticker.FuncFormatter(lambda x, pos: '{0:g}'.format(x + x1))
    ax.xaxis.set_major_formatter(xticks)

    # ax.tick_params(axis='both', which='major', labelsize=20)
    ax.tick_params(axis='both', which='major', labelsize=labelsize)
    spacing = 100.0 # This can be your user specified spacing. 
    minorLocator = ticker.MultipleLocator(spacing)
    minorLocatorY = ticker.MultipleLocator(spacing*2)
    ax.xaxis.set_minor_locator(minorLocator)
    ax.yaxis.set_minor_locator(minorLocatorY)
    # ax.grid(True)
    # ax.grid(which='minor')

    # ax.set_title('After noise removal', fontsize=fontsize)
    ax.set_ylabel('Time [$\mu$s]', fontsize=fontsize)
    ax.set_xlabel(' Wire No.', fontsize=fontsize)
    im = ax.imshow(zt1.T,interpolation='none', origin='lower', aspect='auto',
                    cmap = cmap, vmin=vmin, vmax=vmax) # aspect: height/weight ratio or width/height?


    # fig.suptitle('Run ' + run + ', Event ' + event + ', Signal on ' + plane.upper() + ' plane', fontsize=fontsize+8, y=yloc)
    # plt.tight_layout()
    # moves subplots down slightly to make room for the figure title
    # plt.subplots_adjust(top=1.05)
    fig.subplots_adjust(right=0.86)
    cbar_ax = fig.add_axes([0.88, 0.1, 0.02, 0.80])

    # if plane in ["u"]:
    # 	fig.subplots_adjust(left=0.25)
    # 	fig.subplots_adjust(right=0.86)
    # 	cbar_ax = fig.add_axes([0.86, 0.1, 0.02, 0.80])
    # elif plane in ["v"]:
    # 	fig.subplots_adjust(left=0.10)
    # 	fig.subplots_adjust(right=0.88)
    # 	cbar_ax = fig.add_axes([0.88, 0.1, 0.02, 0.80])
    # elif plane in ["x"]:
    # 	fig.subplots_adjust(left=0.16)
    # 	fig.subplots_adjust(right=0.88)
    # 	cbar_ax = fig.add_axes([0.88, 0.1, 0.02, 0.80])

    # cbar_ax = fig.add_axes([0.90, 0.16, 0.02, 0.74]) # [x0, y0, width, height] of the color bar
    fig.colorbar(im, cax=cbar_ax)
    # cbar_ax.set_ylabel('ADC from baseline', fontsize=fontsize)
    cbar_ax.tick_params(axis='both', which='major', labelsize=labelsize)
    fig.savefig(dirname+'/protodune_evd_'+option+'_temp.pdf')


if __name__ == "__main__":
    #try:
    filename = sys.argv[1]
    option = sys.argv[2]
    x1 = 0
    y1 = 0
    nofy = 4 # rebin
    if len(sys.argv)>3:
        x1 = float(sys.argv[3])
    if len(sys.argv)>4:
        y1 = float(sys.argv[4])
    if len(sys.argv)>5:
        nofy = int(sys.argv[5])

    plot(filename, option, x1,y1, nofy)
    #except:
    #    print('usage:\n\t python evd-subregion [file] [option]')
    #    print('option: orig, raw, decon, etc')





