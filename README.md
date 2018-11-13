# Magnify

## A magnifier to investigate raw and deconvoluted waveforms.

### Usage

```
cd scripts/
root -l loadClasses.C 'Magnify.C("path/to/rootfile")'
# or
root -l loadClasses.C 'Magnify.C("path/to/rootfile", <threshold>, "<frame>", <rebin>)'
```

The second argument is the default threshold for showing a box.

The third, optional argument names which output from the signal processing to display.  Likely names are:

- `decon` produced by the Wire Cell prototype (default).
- `wiener` produced by the Wire Cell toolkit, used to define ROI or "hits".
- `gauss` produced by the Wire Cell toolkit, used for charge measurement.

The call to ROOT can be be called somewhat more easily via a shell
script wrapper.  It assumes to stay in the source directory:

```
/path/to/magnify/magnify.sh /path/to/wcp-rootfile.root
# or
/path/to/magnify/magnify.sh /path/to/wct-rootfile.root 500 gauss 4
```

### Example files

An example ROOT file of waveforms can be found at twister:/home/wgu/Event27.root

If one omits the file name, a dialog will open to let user select the file:
```
cd scripts/
root -l loadClasses.C Magnify.C
```

### Merge six APAs from WCT simulation
```
./magnify-mergesim.sh /absolute/path/to/file/directory/ filename.root
```
The merged file will be available at data/filename-v2.root
