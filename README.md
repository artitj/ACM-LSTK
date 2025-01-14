Accumetra is making available a revised version of the open source Lesion Sizing Toolkit, now called 
ACM-LSTK (stands for Accumetra Lesion Sizing Toolkit). The initial open source LSTK project was 
started and led by Rick Avila, with funding from the Air Force Research Laboratory and the NIH National 
Library of Medicine, was made available to the open source medical imaging community as a module 
within the Insight Segmentation and Registration Toolkit (ITK). The original LSTK project was 
designed to be a flexible architecture for experimenting and implementing volumetric lesion sizing 
algorithms (e.g. CT small lung nodules). The original LSTK team included Xiaoxiao Liu X., 
Brian Helba, Karthik Krishnan, Patrick Reynolds, Matthew McCormick, Wes Turner, Luis Ibáñez, and 
Rick Avila.

This new ACM-LSTK version has been improved to include a much higher performance CT small lung nodule 
volume measurement algorithm. Notably, the small nodule segmentation algorithm now achieves accurate sub-voxel 
nodule segmentation boundaries, which is particularly important for the precise measurement of small 
objects (e.g. lung nodules 6 mm in longest diameter).

This repository contains everything needed to build ACM-LSTK from scratch. The build.sh script contains
all of the commands needed to configure an Ubuntu Linux machine and build the LungNoduleSegmenter 
algorithm. If you have not already installed the necessary ITK, VTK and other libraries (to the required
library version numbers), you should uncomment the appropriate commands at the end of the build.sh script 
and run the script.

You can also find one example open source CT lung dataset (data/E00140) containing two lung nodules visible 
on one slice, one of which is attached to the lung wall:

![E00140 seedpoints](https://user-images.githubusercontent.com/5749559/154883728-0aa1ca28-3213-43bf-a7e0-e8bc3aee5ed9.png)

The file "example_run_commands" shows the arguments used to obtain the images and polygonal surface (*.vtp)
in the data/E00140_output directory:

![result_image_2](https://user-images.githubusercontent.com/5749559/154883797-66209d70-e84c-466a-9060-27ffd0d68d8c.jpg)
![result_image_2](https://user-images.githubusercontent.com/5749559/154883830-cf9d0413-3fb1-449d-96c6-ebb2fb00afb2.jpg)

If you have any questions about ACM-LSTK, please send email to info@accumetra.com.


# Docker

To create a docker image, run the following in the top-level directory:

    docker build -t <desired image name> .

To run the docker image:

    docker run -ti --name <container name> -v <local_folder>:/host -d <image name>

To attach to the container CLI:

    docker exec -ti <container name> /bin/bash

For example, to build an image named lstk:20220220 and do a test using the sample data:

    docker build -t lstk:20220220 .
    docker run -ti --name lstk_container -v <local_path>:/host -d lstk:20220220
    mkdir <local_path>/output
    chmod 777 <local_path>/output
    docker exec -ti lstk_container /home/ubuntu/Dev/ACM-LSTK/src/LungNoduleSegmenter-build/LungNoduleSegmentation --InputDICOMDir "/host/data/E00140/" --Seeds 3 -62 66 -91.5 --MaximumRadius 12.0 --Visualize 1 --Outline 1 --Screenshot /host/output --Supersample 1 --SupersampledIsotropicSpacing 0.2 --WriteFeatureImages 1 --OutputMesh /host/output/E00140.N1.vtp
