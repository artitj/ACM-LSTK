Accumetra is making available a revised version of the open source Lesion Sizing Toolkit, now called 
ACM-LSTK (stands for Accumetra Lesion Sizing Toolkit). The initial open source LSTK project was 
started and led by Rick Avila, with funding from the Office of Naval Research and the NIH National 
Library of Medicine,was made available to the open source medical imaging community as a module 
within the Insight Segmentation and Registration Toolkit (ITK). The original LSTK project was 
designed to be a flexible architecture for experimenting and implementing volumetric lesion sizing 
algorithms (e.g. CT small lung nodules). The original LSTK team included Xiaoxiao Liu X., 
Brian Helba, Karthik Krishnan, Patrick Reynolds, Matthew McCormick, Wes Turner, Luis Ibáñez, and 
Rick Avila.

This new ACM-LSTK version has been improved to include a much higher performance CT small lung nodule 
volume measurement algorithm. Notably, the small nodule segmentation algorithm now achieves sub-voxel 
nodule segmentation boundaries, which is particularly important for the precise measurement of small 
objects (e.g. lung nodules 6 mm in longest diameter).

Prerequisites for compiling ACM-LSTK are the following libraries:
* ITK version X
* VTK version Y


