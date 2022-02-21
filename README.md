Accumetra is making available a revised version of the open source Lesion Sizing Toolkit, now called 
ACM-LSTK (stands for Accumetra Lesion Sizing Toolkit). The initial open source LSTK project was 
started and led by Rick Avila, with funding from the Office of Naval Research and the NIH National 
Library of Medicine, was made available to the open source medical imaging community as a module 
within the Insight Segmentation and Registration Toolkit (ITK). The original LSTK project was 
designed to be a flexible architecture for experimenting and implementing volumetric lesion sizing 
algorithms (e.g. CT small lung nodules). The original LSTK team included Xiaoxiao Liu X., 
Brian Helba, Karthik Krishnan, Patrick Reynolds, Matthew McCormick, Wes Turner, Luis Ibáñez, and 
Rick Avila.

This new ACM-LSTK version has been improved to include a much higher performance CT small lung nodule 
volume measurement algorithm. Notably, the small nodule segmentation algorithm now achieves sub-voxel 
nodule segmentation boundaries, which is particularly important for the precise measurement of small 
objects (e.g. lung nodules 6 mm in longest diameter).

This repository contains everything needed to build ACM-LSTK from scratch. The build.sh script contains
all of the commands needed to configure an Ubuntu Linux machine and build the LungNoduleSegmenter 
algorithm. If you have not already have installed the necessary ITK, VTK and other libraries (to the required
library version numbers), you should uncomment the appropriate commands at the end of the build.sh script 
and run the script.

You can also find one example open source CT lung dataset (data/E00140) containing two lung nodules visible 
on one slice, one of which is attached to the lung wall. The file "example_run_commands" shows the arguments 
used to obtain the images and polygonal surface (*.vtp) in the data/E00140_output directory. 

If you have any questions about ACM-LSTK, please send email to info@accumetra.com.
