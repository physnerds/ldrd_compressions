import numpy as np
import h5py,random

#python function that generates waveform data 
# frequency, amplitude, phase shifts as parameters

def create_waveform(frequencies, amplitudes, phases):
    sampling_rate = 1000 #how often to record the DAQ
    duration = 2 #Duration in seconds...
    #the time values...
    time_val = np.linspace(0,duration, int(sampling_rate*duration),endpoint=False)

    y_tot = np.zeros_like(time_val)
    for freq, amp, phase in zip(frequencies,amplitudes,phases):
        y = amp*np.sin(2*np.pi*freq*time_val+phase)
        y_tot += y 
    return y_tot


def write_waves(n_waves):
    #Create frequencies for third to n_waves wave
    rand_freq = np.random.randint(3,n_waves,size=(1,n_waves-2))
    sum_freq = np.sum(rand_freq)
    avail_phases = [np.pi/2,3*np.pi/2,0,np.pi/4]
    phases = []
    for i in range(0,n_waves-2):
        phases.append(random.choice(avail_phases))
    
    #now the amplitude...
    amplitudes = []
    for i in range(0,n_waves-2):
        amplitudes.append(float(rand_freq[i])/sum_freq)
    





# Here we try to write the wave form...
#first create a hdf5 file
filename = "waveforms.h5"
n_minor_waves = 6
hf = h5py.File(filename,'w')

frequencies = [1, 2, 3,5,8,12,15]
amplitudes = [1, 0.7, 0.2,0.1,0.05,0.05,0.05] 
phases = [3*np.pi/4, np.pi, np.pi/2,np.pi/2,0,0,np.pi/4]

data = create_waveform(frequencies, amplitudes, phases)
hf.create_dataset('waveforms',data=data)
