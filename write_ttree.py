import h5py,os,sys
import ROOT
from waveforms import waveforms
import numpy as np

def writetoROOT(dataset,bname,rtree):
    rdata = np.zeros(1,dtype=type(dataset[0]))
    rtree.Branch(bname,rdata,bname+"/D")

    for val in rdata:
        rdata[0] = val
        rtree.Fill()
    rtree.Write()
    

filename = "waveforms.h5"
n_data = 500
n_waves = 7
hf = h5py.File(filename,'r')
wf = waveforms(hf,n_data,n_waves)

#create ROOT file and TTRee
root_file = ROOT.TFile(filename.replace(".h5",".root"),"RECREATE","",0) #0 means to compression
tree = ROOT.TTree("waveforms","Tree with Waveforms data")
for i in range(0,n_data):
    _name = "waveform_"+str(i)
    dataset = wf.read_dataset(_name)
    writetoROOT(dataset,_name,tree)

comp_algo = root_file.GetCompressionAlgorithm()
comp_level = root_file.GetCompressionLevel()

print("Compression Algorithm of the ROOT File ",comp_algo)
print("Compression Level of the ROOT File ",comp_level)

root_file.Close()
