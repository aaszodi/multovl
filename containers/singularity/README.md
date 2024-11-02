# MULTOVL in a Singularity container

Version 1.5
November 2024

By convention the Singularity recipes have a `.def` extension,
and the image files `.sif`. The latter are `.gitignore`-d.

For experiments you can use Singularity sandboxes under the `sandboxes`
subdirectory. This contains a `.gitignore` file so that the directory itself
is under source control, but all contents will be ignored.

## Building the Singularity image

Build the image locally:

`sudo singularity build multovl.sif multovl.def`

Check if everything is all right:

```
singularity test multovl.sif
singularity exec multovl.sif multovl ../../src/tests/data/triple[a-c].bed
```

## Uploading the image

Sign the image:

`singularity sign multovl.sif`

...and then upload it. The example below uses AA's Sylabs repo.
Tag the image with the Multovl version (currently 1.5)

```
singularity remote login
singularity push multovl.sif library://andras.aszodi/default/multovl:1.5
```

## Run the MULTOVL container

The instructions below assume you have access to an HPC cluster
with Singularity/Apptainer installed. The submission system is assumed to be SLURM.

First download the MULTOVL image:

`singularity pull library://andras.aszodi/default/multovl`

This will create a file `multovl_latest.sif` in your working directory.

Then write a SLURM job submission file that contains the following line:

```
singularity exec multovl_latest.sif multovl [options] infile1.bed infile2.bed ... > outfile.gff
```

where `[options]` are the command-line options (refer to `multovl --help`),
the files `infile1.bed infile2.bed ...` are the input files, and `outfile.gff`
is the result. Change these according to your needs.

When booking SLURM resources keep in mind that the standard `multovl` app
runs on a single core. Only the `parmultovlprob` application
can make use of several cores.
