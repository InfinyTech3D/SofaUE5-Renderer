# SofaUE5-Renderer

## Description
This repository is a **Unreal Engine 5 plugin allowing to render a SOFA simulation scene inside UE5**. 
All SOFA `VisualModel` components present in the loaded simulation will be rendered inside UE5 engine as `Actor` with a `GeneratedMesh` instance.  Thus, it is possible to apply Unreal Engine Materials to each visual model and also to combine UE5 rendering pipeline with the SOFA simulation.
<br>

<table>
<tr>
    <td style="text-align: center; vertical-align: middle;"><strong>Important:</strong> This version allows only to change the Gravity and the TimeStep of the SOFA simulation and only the <i>VisualModel</i> are mapped into UE5. <br>A <strong>full integration of SOFA</strong> components with a two-way communication will soon be avaible.</td>
</tr>
</table>


#### Compatibility:
- Unreal Engine version 5.0.3
- SOFA version >= 22.12 with SofaPhysicsAPI 
- Only tested on Windows

## Installation guide
1. Install Unreal Engine version 5.0.3
2. (optionnal) Create a new project
3. clone the repo inside your Unreal Engine project: 
```git clone git@github.com:InfinyTech3D/SofaUE5-Renderer.git /UnrealEngine/myUE5Project\Plugins\SofaUE5-Renderer```
5. Copy the SOFA dll libraries inside: ```/UnrealEngine/myUE5Project\Plugins\SofaUE5-Renderer/Binaries/ThirdParty/SofaUE5Library/Win64/```  (for windows)
6. Restart Unreal Engine and reopen your project. The new plugin should be there.

## Usage
Once the plugin has been loaded you should have this folders structure under Plugins/

<p align="center">
	<img src="https://github.com/InfinyTech3D/SofaUE5-Renderer/blob/master/Doc/img/menu_plugin_folder.jpg" style="width:400px;"/>
</p>

- SofaUE5 Content contains several examples listed below with their corresponding SOFA scene as well as textures and Materials.
- SofaUE5 C++ Classes provide the components to be used to add SOFA simulation inside the UE scene

1. Drag and Drop the SofaContext C++ class into your current level. 
This component will be added to the scene and can be access inside the Outliner. This compoment will create a SOFA simulation physically represented by this Actor in the Level.

<p align="center">
	<img src="https://github.com/InfinyTech3D/SofaUE5-Renderer/blob/master/Doc/img/menu_SofaContext_creation.jpg" style="width:400px;"/>
</p>

You can access the SofaContext parameters by clicking on it and change its parameters. You can scale or move the SOFA frame to match your current scene.
Other parameters linked to SOFA can also be changed like the gravity vector as well as the time steepping and activating the logs.

<p align="center">
	<img src="https://github.com/InfinyTech3D/SofaUE5-Renderer/blob/master/Doc/img/menu_SofaContext_parameters.jpg" style="width:400px;"/>
</p>

2. **Click on the button** next to the ```File Path```  to load a SOFA scene. This will create an `Actor` SofaVisualMesh with a `GeneratedMesh` for each SOFA ```VisualModel```

SOFA simulation will be performed as soon as Unreal Engine is playing.

## Examples
Three examples corresponding to SOFA Demo folder are provided inside the package:
They are available in the folder [/Scenes/Demos/](https://github.com/InfinyTech3D/SofaUE5-Renderer/tree/master/Content/)
- Demo_01_SimpleLiver.umap -> Integration of  [Content/SofaScenes/liver.scn](https://github.com/InfinyTech3D/SofaUE5-Renderer/blob/master/Content/SofaScenes/liver.scn)
- Demo_02_Caduceus.umap -> Integration of  [Content/SofaScenes/caduceus.scn](https://github.com/InfinyTech3D/SofaUE5-Renderer/blob/master/Content/SofaScenes/caduceus.scn)
- Demo_03_Tissue.umap -> Integration of  [Content/SofaScenes/tissue.scn](https://github.com/InfinyTech3D/SofaUE5-Renderer/blob/master/Content/SofaScenes/tissue.scn)

Here are a some results of the integration:
<p align="center">
	<img src="https://github.com/InfinyTech3D/SofaUE5-Renderer/blob/master/Doc/img/liver.jpg" style="width:1000px;"/>
</p>
<p align="center">
	<img src="https://github.com/InfinyTech3D/SofaUE5-Renderer/blob/master/Doc/img/SofaUnreal.jpg" style="width:1000px;"/>
</p>

## License
This Unreal Engine plugin is under GPL-3 license. 
<br>
Other license formats can be provided for commercial use. For more information contact: contact@infinytech3d.com.


