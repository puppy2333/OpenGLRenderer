# OpenGLRenderer
Hi there! This is OpenGLRenderer, my tiny renderer that implements some widely-used rendering (and also simulation) algorithms.

Currently this repo is only for demo purpose, so some 3rd-party libs are not included in the repo.
If you want, you can add them manually according to CmakeList.
All the core algorithm are included.

Here is what I have done.

## Defered Rendering
Defered rendering is hard to demo, so it will be left blank here.

## Screen space ray tracing
I use specular reflection to demonstrate the effect of screen space ray tracing.
Here is an example of a small cabin being reflected by a mirror on the ground.

<img src="imgs/ssr.png" alt="ssr" width=45%/>

## Soft shadows
Three types of shadows are implemented: vanilla shadow, percentage closer filter (pcf) shadow and percentage closer soft shadow (pcss) shadow.
Here is a demo (left: vanilla shadow, middle: pcf, right: pcss).

<img src="imgs/shadow.png" alt="shadow" width=32%/>
<img src="imgs/shadow_pcf.png" alt="shadow_pcf" width=32%/>
<img src="imgs/shadow_pcss.png" alt="shadow_pcss" width=32%/>

## Screen space ambient occlusion
Here is a demo of screen space ambient occlusion (ssao).
left: no ssao, middle: wihth ssao, right: visualization of ssao color buffer.

<img src="imgs/ssao_no.png" alt="shadow" width=32%/>
<img src="imgs/ssao.png" alt="shadow" width=32%/>
<img src="imgs/ssao_vis.png" alt="shadow" width=32%/>

## Shallow water equation
I also added a simple demo of shallow water equation (swe) using fragment shader (the demo is supposed to run on all platforms, and macos doesnot support OpenGL compute shader).

<img src="imgs/swe_1.png" alt="shadow" width=24%/>
<img src="imgs/swe_2.png" alt="shadow" width=24%/>
<img src="imgs/swe_3.png" alt="shadow" width=24%/>
<img src="imgs/swe_4.png" alt="shadow" width=24%/>

