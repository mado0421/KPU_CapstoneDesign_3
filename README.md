# 종합설계

### 주요 목표

- [x] 빛과 그림자 구현
- [x] 애니메이션 구현
- [x] 포스트 프로세싱 구현(블러 등)
- [x] 충돌검사 및 후처리

# 조작법

- **WASD**로 이동합니다.
- **마우스 좌우 이동**으로 보는 방향을 조절합니다.
- **마우스 좌클릭**으로 사격합니다.
- **ESC**로 프로그램을 종료합니다.
- **M**으로 마우스 고정을 켜고, **N**키로 마우스 고정을 끕니다.

## 구현 내용

* **FBX Mesh Rendering**(Diffuse, Normal, Roughness)
* **Deferred Shading Rendering Path**(RenderToTexture, GBuffer 등)
* **Spot Light & Shadow Map**
* **Point Light &** (GS를 이용한)**Cube Shadow Map**
* **Directional Light &** (GS를 이용한)**Cascade Shadow Map**
* **FBX Mesh & Animation Info Parser**
* **Animation Blend** (+**보행이동 기반 블렌딩**)
* (CS를 이용한)**Blur, HDR, Bloom PostProcess**
* **Component** 구조
* **Collision Detection & Solve Constraint**
* **충돌 시 벽면에 미끄러짐**
* **Particle**(+Particle Emitter) **& Effect**

-----------

### 21.03.15 - 21.03.21

#### 이번주 일정

* 조명별 ShadowMap 생성 및 업로드(64개의 ShadowMap을 올릴 수 있도록)
* SpotLight Shadow 구현
* Cubemap과 GS를 이용한 PointLight Shadow 구현
    - Cubemap Texture 사용법 숙지
    - GeometryShader 사용법 숙지
* Cascade ShadowMap 을 이용한 Directional Light Shadow 구현
    - Texture Array 사용법 숙지

#### 2주 목표

* 그림자 구현
* 바닥 생성
* 큐브가 아닌 모델을 .obj 포맷으로 불러와서 렌더링

#### 현재 구현된 사항

- 큐브 메쉬 렌더링(uv, normal, tangent 정보를 가진)
- DeferredShading을 위한 2 Pass 렌더링
    - Render To Texture
    - DepthBuffer 읽기
    - Texture를 ShaderResource로 사용하여 두 번째 Pass에서 worldPos, normal, color 정보 언패킹하기
    - 언패킹한 정보를 바탕으로 Light 계산하기

### 2021.03.18

<img src="https://user-images.githubusercontent.com/21697638/111553756-58535f80-87c8-11eb-95b5-4fb1c6bab7a7.png" width="70%" height="70%"></img>

-----------

### 21.03.22 - 21.03.28

#### 이번주 일정

* Cubemap과 GS를 이용한 PointLight 그림자 구현
    - Cubemap Texture 사용법 숙지
    - GeometryShader 사용법 숙지
* Cascade ShadowMap 을 이용한 Directional Light 그림자 구현
    - Texture Array 사용법 숙지

#### 2주 목표

* 그림자 구현
* 바닥 생성
* 큐브가 아닌 모델을 .obj 포맷으로 불러와서 렌더링

#### 현재 구현된 사항

- 큐브 메쉬 렌더링
- 디퍼드 쉐이딩
- 다중 SpotLight & Shadow

Root Constant를 한 프레임 내에 여러 번 바꿀 수 있을 것이라 오해하고 있었다.
말 그대로 상수처럼 사용되는 것인데.
그래서 인덱스를 갱신해가며 사용할 수 있을 것이라 믿었던 Light와 ShadowMap 배열 구조를 전부 부수고 광원별 렌더 패스를 추가하는 식으로 변경하였다.

### 2021.03.21

<img src="https://user-images.githubusercontent.com/21697638/111884756-f6cd0400-8a06-11eb-90d6-d70c830bd86f.png" width="70%" height="70%"></img>

### 2021.03.22

프로젝트의 단위 1 이 1 m가 되도록 설정하였다. (0.001 = 1mm, 1,000 = 1km)
투영 변환 행렬도 그에 맞게 조절하고 Object와 Light의 크기도 변경하였다.
이제 Load Mesh가 된다.

<img src="https://user-images.githubusercontent.com/21697638/111926071-aa5cf380-8aee-11eb-83ce-9b9e3ea0e60d.png" width="70%" height="70%"></img>

### 2021.03.25

Point Light Shadow를 위해 TextureManager와 Texture에 CubeMap Type으로 Texture를 생성하는 함수를 추가해주고 Header.hlsli에 TextureCube를 추가해주었다.
원래 Texture2DArray로 만들고 그걸 Texture2D로 되지 않을까 하고 올려봤었는데 생각대로 작동하지 않았다.
[Introduction To Textures in Direct3D 11 - Win32 apps | Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-resources-textures-intro)
여기에 보면 CubeMap의 인덱스가 나온다. 저 순서대로 Light마다 뷰 변환 행렬을 만들어줘야 한다(올릴 땐 투영 변환 행렬 곱하고 전치행렬로 바꿔서).

<img src="https://user-images.githubusercontent.com/21697638/112391901-f2248a80-8d3b-11eb-8d31-36f302a8be15.png" width="70%" height="70%"></img>

-----------

### 21.03.29 - 21.04.04

#### 이번주 일정

* Cascade ShadowMap 을 이용한 Directional Light Shadow 구현
* Render Object를 할 때, Texture 등을 어떻게 저장할 것인지 정리(자료구조 면에서)
* Material 구조체 업로드 준비
* FBX SDK로 파일 읽기(애니메이션 구현 준비)

#### 2주 목표

* Object 자료구조 정리(또는 Component 인터페이스 작성)
* FBX SDK 다운로드 및 사용법 정리

#### 현재 구현된 사항

- 큐브 메쉬 렌더링
- DeferredShading
- 다중 SpotLight Shadow
- 다중 PointLight Shadow
- Cascade Shadow Map을 TextureArray를 사용하여 DepthBuffer에 그리는 것까지 진행

### 2021.03.29

26일에 DepthBuffer에 그려지는 것까지 확인

### 2021.03.31

CSM 구현 시에 많은 문제가 있었는데 현재 진행 상황은 다음과 같다.

- 뷰 프러스텀의 꼭짓점 좌표를 구하기 위해 CameraView 변환 행렬의 역행렬을 곱해 WorldSpace로 가져오고, LightSpace로 변환한 뒤에, 각 캐스케이드 별로 min/max XYZ를 구하여 직교 투영 행렬을 만들 때 필요한 width와 height, Znear, Zfar을 구했다.
- 각 캐스케이드 별로 위치를 구하여 View 변환 행렬을 만들어주고 위에서 구한 투영행렬과 곱해서 GPU에 올려줬다.
- 그래서 일단 되긴 한다
- 다만, 카메라 회전 시에 ShadowMap 안에 안 들어가는건지 아니면 캐스케이드 인덱스를 구하는 식에 문제가 있는건지 중간에 의도하지 않은 그림자가 생기는 문제가 있다

<img src="https://user-images.githubusercontent.com/21697638/113150035-9cd20700-926e-11eb-9230-03f8149263ce.png" width="70%" height="70%"></img>
<img src="https://user-images.githubusercontent.com/21697638/113150117-b4a98b00-926e-11eb-96b3-3f682aa57016.png" width="70%" height="70%"></img>

### 2021.04.01

CSM을 구현했다. 구현하면서 있었던 문제들과 해결법은 다음과 같다.

1. DepthTextureArray에 렌더는 되는데 카메라가 이동해도 DepthTexture의 내용은 바뀌지 않음 -> Light의 위치를 각 캐스케이드의 시작점으로 이동시켜 해결
2. 거리에 따라 잘못된 캐스케이드가 선택되어 의도하지 않은 그림자가 발생 -> 월드 공간에서 픽셀의 위치와 카메라 간의 거리로 캐스케이드를 선택하게 하지 않고, 월드 공간 픽셀의 위치를 카메라 공간으로 옮긴 뒤에 z값으로 캐스케이드를 선택하게 변경(원래 캐스케이드의 바운더리 박스를 만들 때 쓰는 뷰 프러스텀도 카메라 뷰 공간에 있었으니까)
3. 깊이텍스처의 일부(오른쪽과 아래쪽) 공간에 렌더링이 되지 않음 -> 이건 다른 쉐도우 맵을 생성할 때도 있었으나 평상시에는 눈에 띄지 않아서 몰랐던 문제로서 쉐도우 맵을 렌더링 할 때, 뷰 포트의 크기만 쉐도우 맵의 크기에 맞게 변경해주고 시저렉트는 변경해주지 않아서 생긴 문제였음(쉐도우 텍스처의 크기를 2048로 바꾸면 1/4만 그려지는 문제가 있었는데 아... 암튼 해결함)

이렇게 적어보면 문제가 적어보이지만 적합한 디버깅 방법을 알지 못했던 터라 주먹구구식으로 시도한 일이 많았다. 효율적인 디버깅 방법이 필요하다.

이제 Object 자료구조와 Material 등을 정리하고 구현한 뒤에 애니메이션으로 넘어갈 준비를 하면 될 것 같다.

<img src="https://user-images.githubusercontent.com/21697638/113239609-0b05e080-92e6-11eb-8cce-9dda85a663b9.png" width="70%" height="70%"></img>
<img src="https://user-images.githubusercontent.com/21697638/113239658-253fbe80-92e6-11eb-99b4-921ac5e872fa.png" width="70%" height="70%"></img>

### 2021.04.04

나는 더 이상 3ds Max를 사용할 수 없고, 내가 원하는 (diffuseMap, normalMap, specularMap이 있고 아마 앞으로 애니메이션까지 들어가면 더 까다로워질)조건으로 완벽하게 준비된 에셋을 구하기도 어려울 것 같아서 Blender 2.92로 에셋을 만들었다. Blender가 3ds Max와 사용방법이 많이 달라서 적응하는데 오래 걸렸다.

<img src="https://user-images.githubusercontent.com/21697638/113578416-c23f8600-965d-11eb-8275-8df5d09a4e3a.png" width="70%" height="70%"></img>

-----------

### 21.04.05 - 21.04.11

#### 이번주 일정

* Object 렌더링 시에 Texture 등을 어떻게 저장할 것인지 정리(자료구조 측면에서)
* Material 구조체 업로드 준비
* FBX SDK로 파일 읽기(애니메이션 구현 준비)

#### 2주 목표

* Object 자료구조 정리(또는 Component 인터페이스 작성)
* FBX SDK 다운로드 및 사용법 정리

### 2021.04.05

자원 관리 자료구조를 작성했다.
원래 Scene에 속해있던 TextureMng와 Object에 속해있던 vector<mesh*>의 자원들을 전역으로 올리고 한 번씩만 생성하게 변경하였다.(원래는 같은 mesh여도 여러 Object일 경우, 여러 번 생성하여 메모리 낭비가 있었다)
따라서 지금 구조는 다음과 같다.

> 전역에 gTextureMng, gModelMng, gMaterialMng 로 자원관리
> 처음에 AssetData를 읽어서 이 Scene에서 어떤 Asset(obj, dds 등)을 사용하는지 확인
> 위에서 확인한 AssetList를 바탕으로 Model(mesh들의 집합)과 Texture를 Load
> MaterialData를 읽어서 Material을 생성
> (defaultMaterial도 생성하여 mat이 없는 object의 경우, defaultMaterial을 사용하게 함)
> Object에서 Render를 할 때, Material과 Model name을 가지고 gMaterialMng, gModelMng에서 Set과 Render하게 변경
> (이전에 사용한 Material과 같은 경우, 추가로 Set을 하지 않게 이전에 사용한 mat의 이름을 Mng에서 저장)

자원 관리를 위해 Data 파일만 3개를 사용하게 됨
자원 사용을 위해선 다음의 순서를 거쳐야 함

1. 먼저 Data 폴더의 AssetList에 사용할 에셋을 추가한다
2. 해당 에셋을 Assets 폴더에 추가한다
3. MaterialData에 추가할 Texture를 더해준다
4. ObjectData에 Material과 Model을 더해준다

SpecularMap을 추가하였다
스펙큘러맵을 이렇게 사용하는게 맞는진 모르겠으나 일단 0~1 사이의 값을 가지고 roughness 값으로 사용된다. 이게 있으면 한 Texture를 사용하는 Model에서 다양한 roughness 값을 줄 수 있게 된다
지금 light를 계산하는 hlsl에서 roughness * 256 해서 사용하고 있으므로 기억해두자
Fresnel값도 어떻게 주고 싶은데 그것까지는 아직 못해주고 있다
추가로 RGB Texture를 써야 할 지 고민해봐야 할 것 같음

<img src="https://user-images.githubusercontent.com/21697638/113580174-45fa7200-9660-11eb-841f-0f87b0694b88.png" width="70%" height="70%"></img>

### 2021.04.08

Blender에서 리깅과 애니메이션 작업까지 한 모델을 FBX로 Export하여 사용하기 위해 작업했다.
스키닝 애니메이션은 한 차례 구현해 본 적이 있어 이론을 복습하는 것은 쉬웠다.
FBX SDK의 사용법이 어렵고 복잡해서 많이 힘들었는데 Parser를 작성한 지금도 현재의 방식이 그리 맘에 들지 않는다.
Blender에서 키프레임을 Export하고 다시 Import 해보니 Max에서 리샘플링 옵션을 켠 것처럼 모든 프레임에 키프레임이 생기는걸 볼 수 있었다. 특정 키프레임의 시간만 얻는 것을 포기하고 모든 프레임에서 toWorld 행렬을 뽑아냈다.
빠르게 로드하기 위해 바이너리 포맷으로 만든 뒤에 한 번에 데이터 전체를 읽어내는 식으로 만들었다. (이후 처리 공정이 있긴 하지만)
지금은 루트 상수버퍼를 추가하고 애니메이션 본 변환 행렬을 올리는 것까지 진행했다.
앞으로 해야 할 일은 다음과 같다.

1. 변환 행렬을 시간에 따라 보간.
2. 이 오브젝트가 FBX 모델인지 OBJ 모델인지, 애니메이션이 적용되어있는 메쉬인지 등을 구분.
3. 각자 다른 PSO를 사용하여 렌더링.

Blender에서 FBX로 Export 할 때, Topology를 D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP이 아니라 D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST로 뽑고 싶은데 Export 옵션에선 못 찾겠다.
pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
이 함수를 찾아서 수정해야 할 것. (FBX모델은 TRIANGLESTRIP로 변환했으니까)

-----------

### 21.04.12 - 21.04.18

#### 이번주 일정

* AnimationClip, AnimationCtrl 등 Animation 자료구조 구현

#### 2주 목표

* Animation 구현

### 2021.04.12

Blender에서 Export한 FBX 파일에 문제가 많아서 다시 3ds Max로 작업환경을 변경했다. Blender에서 있었던 문제는 다음과 같다.

- FBX 포맷으로 Export 할 경우, Vertex가 없어지는 문제가 있다. 정확히는 프리미티브 토폴로지가 트라이앵글도 아니고 트라이앵글 스트립도 아니고 전체 Face의 절반이 사라지는 문제가 있다. Export 한 결과물을 다시 Import 했을 때 Mesh에 문제가 있던 것 같아 보이진 않았는데 대체 어떻게 저장을 하고 있는 것인지 모르겠음.
- Export 할 때, 3ds Max에서의 '리샘플링 올' 옵션이 자동으로 적용되는 듯하다. Keyframe의 Time 값을 얻기 위해 Parser를 돌렸을 때, 모든 프레임의 시간이 나오길래 뭔가 싶어서 다시 Import 해봤는데 모든 프레임에 키프레임이 찍혀있는걸 보고 3ds Max를 설치했다.

<img src="https://user-images.githubusercontent.com/21697638/114394625-5ec1d500-9bd6-11eb-8125-32ac869a7680.png" width="70%" height="70%"></img>

Blender에서 Export 하는 것은 문제가 너무 많아서 다시 Max에서 작업하는 것으로 변경했다. 그로 인해 AnimationClip의 자료구조가 변경되었고(이젠 키프레임을 리샘플링 올을 하지 않은 결과로 처리할 수 있으니까) Parser의 구조도 변경하였다.

AnimationClip의 구조는 다음과 같다.

    struct Keyframe {
	    float keytime;
	    float4 rotation;
	    float3 translation;
	}
	
	using Bone = vector<Keyframe>;
	
	AnimationClip DataStructure
	
	nBone
	eachBone: nKeyframe
	sizeof(Keyframe) * nTotalKeyframe

Parser를 구현했고 이에 맞춰서 본 프로젝트에서 Import 할 수 있게끔 변경했다. FBX로 읽은 Mesh를 렌더링 하는 것까지 구현했다. AnimationClip을 읽어오는 것까지 성공했다.

### 2021.04.13

AnimationClip에서 읽은 FrameData를 GPU에 올려 VS에서 사용할 수 있게 하였다.
VS Input Semantic으로 TEXCOORD는 되고 TEXCOORD1은 안된다.
FrameData가 문제 없이 올라가는 것을 확인했고 이전 Frame과 다음 Frame 간의 보간을 구현하였다.
3ds Max에서 회전 값은 TCB, 위치 값은 베지어 방식으로 보간한다고 하여 찾아보았는데,
TCB는 Tension, Continuity, Bias의 줄임말로 CatmullRom Spline에 bias 값을 넣은 정도로 보였다.
나는 T, C, B 모두 Default 값으로 두고 에셋을 만들 예정이므로 CatmullRom 보간만 할 수 있으면 된다.
그리고 그건 XMVectorCatmullRom() 라는 함수로 이미 구현되어 있다.

다만, CatmullRom 스플라인은 보간하려는 두 값의 연속성? 미분값? 속도?값 을 정하기 위해 그 앞과 뒤의 값도 필요로 하기 때문에 값을 4개 전달해야 한다.
이것과 지금 AnimationClip의 저장방식(frameLength와 같은 keytime을 가지는 맨 마지막 키프레임이 있을 수도, 없을 수도 있는 구조)에 따른 루프 애니메이션과 루프가 아닌 애니메이션의 키프레임 인덱스를 정하는 방법을 전부 구현하지는 않았다.
지금 구현한 방법은 맨 앞과 맨 마지막이 이어지지 않는다는 가정 하에 구현하였다.

현재 프레임 인덱스를 구하는 것 자체는 크게 문제가 없는 것 같고 변환 행렬 값에 문제가 있어서 제대로 애니메이트 되지 않고 있다.

-----------

### 21.04.12 - 21.04.25

#### 이번주 일정

* Animation Parser, Animation Controller 구현

-----------

### 21.04.26 - 21.05.02

#### 이번주 일정

* 휴식

-----------

### 21.05.03 - 21.05.09

#### 이번주 일정

* Animation Parser, Animation Controller 구현

### 2021.05.03

Fbx Parser 구현 성공. 추후 내용 정리해서 올릴 것.

### 2021.05.04

본 프로젝트에 적용 완료. 코드 정리해서 올릴 것.

<img src="https://user-images.githubusercontent.com/21697638/116945821-8840be80-acb3-11eb-8188-742572a047b6.png" width="70%" height="70%"></img>

- Parser에서 VertexNormal이 제대로 입력되지 않아 Split per-vertex Normals 옵션을 켤 수 없었는데 해당 부분을 수정.
- PSO를 변경하고 추가하여 애니메이션이 적용된 오브젝트도 Shadow Occluder로 작용.

<img src="https://user-images.githubusercontent.com/21697638/116962876-3fecc500-ace2-11eb-8b04-df5c9c8aa109.png" width="70%" height="70%"></img>

### 2021.05.06

미리 만들어둔 사람 모델을 리깅하여 사용하였다.

Parser에서 Mesh를 저장할 때 하나의 Vertex에 영향을 주는 Bone이 5개 이상인 경우가 나오는 문제가 있었다.
하나의 Vertex에 영향을 주는 Bone이 4개를 초과하는 경우, 가장 작은 weight값을 가진 Bone을 지우고 해당 weight값을 나눠 다른 Bone들의 weight에 더해주는 방법으로 수정하였다.

<img src="https://user-images.githubusercontent.com/21697638/117238481-edd5ac00-ae67-11eb-9259-b6ca4016ee7f.gif" width="70%" height="70%"></img>

이전에 구현해둔 쿼터니언 보간 방식은 각 요소별로 CatmullRom 보간을 하는 방법이었기 때문에, 회전값이 튀는 문제가 있었다.
같은 회전값을 나타내더라도 표현하는 쿼터니언 값이 다를 수 있던 문제였는데(아마?) 이를 요소별로 단순하게 보간하여 발생한 것 같아 XMQuaternionSlerp()을 사용하여 보간하게 변경하였다.

FBX Asset Import 시에

1. position과 normal의 Y와 Z를 Swap한다.
2. position과 normal의 Z에 -1을 곱한다.

VertexShader에서

1. position과 normal 변환결과값의 X에 -1을 곱한다.
   (tangent나 uv에 비슷한 처리를 해야 하는지는 아직 모름)

<img src="https://user-images.githubusercontent.com/21697638/117239997-d3e99880-ae6a-11eb-99d1-6542dd5d441b.gif" width="70%" height="70%"></img>

### 2021.05.07

AnimationController를 상속받는 HumanoidAnimCtrl를 만들고 HumanoidObject가 갖고 있는 HumanoidState에 따라 다른 Clip을 재생하도록 하였다.

<img src="https://user-images.githubusercontent.com/21697638/117376845-f2a06b80-af0c-11eb-8174-a37296c1df5d.gif" width="70%" height="70%"></img>

AnimationController에서는 입력받은 State와 Time으로 행렬을 만들어 Set까지 하고 있다.
이것으로 컨트롤러에서 할 일은 일단 끝. (블렌딩 등을 제외하고)
AnimatedObject(특히 HumanoidObject를 기준으로)에서 State 전환을 만들어야 한다.
외부에서 Object에게 MoveForward() 명령어를 주었을 때, 만약 해당 Object가 직전에 Jump를 하여 InAir State라면, MoveForward()를 호출해도 아무런 효과가 없어야 한다.

-----------

### 21.05.10 - 21.05.16

#### 이번주 일정

* State와 Transaction 구현
* Object의 상황에 따라 State가 전환
* 소스코드 정리

#### 2주 목표

* UI와 Font 구현(한글까지 지원할 수 있도록)

### 2021.05.10

State 클래스를 다음과 같이 작성하고 Humanoid_Idle, Humanoid_Walk 클래스를 State 클래스를 상속받아 구현하였다.

	class State {
	public:
		State(const char* strName, Object* obj) :m_strStateName(strName), m_pObject(obj) {}

		void AddTransation(Transaction* transation) { 	m_vecTransactions.push_back(transation); }
		bool IsSatisfyTransaction(const char* strStateName);
		void ChangeStateTo(const char* strStateName);

		virtual void EnterState() {}
		virtual void LeaveState() {}
	public:
		virtual void MoveForward() = 0;
		virtual void Update(float fTimeElapsed) = 0;

	public:
		string m_strStateName;

	protected:
		vector<Transaction*>	m_vecTransactions;
		Object*					m_pObject;
	};

Idle State인 플레이어 캐릭터에게 WalkForward() 명령을 주면 Walk State로 전환할 수 있는 Transaction이 있는지 검사하고, 있으면 해당 Transaction의 조건에 적합한지 확인 후에 맞으면 해당 State로 전환한다.

	if(IsSatisfyTransaction("strStateName")) ChangeStateTo("strStateName");

### 2021.05.11

이번 2주 목표를 'UI와 Font 구현(한글까지 지원할 수 있도록)'에서 '손에 무기 들려주기, IK 구현, 애니메이션 블렌더(이건 시도까지만)'으로 변경
Normal Mapping을 구현하는 것을 먼저 한다.

https://www.textures.com/ 에서 albedo, normal, roughness맵을 다 갖고 있는 Texture Set를 받아 테스트에 사용하였다.
사용한 Set은 [Medieval Cobblestone Pavement - PBR0024 (textures.com)](https://www.textures.com/download/PBR0052/133087)

Specular Map과 Roughness Map이 갖는 의미의 차이점을 잘 모르겠기에 지금은 Specular Map의 슬롯을 기본으로 비워두고, Albedo Map(이것도 Diffuse Map과의 차이점을 모르겠다)의 Alpha 채널에 Roughness Map의 정보를 넣어 하나로 합친 뒤에 사용하였다.
즉, 지금 Material이 Texture Set으로 갖는 정보는 Diffuse.rgb, Roughness.r(==Diffuse.a에 들어감), Normal.rgb 이렇게 있다.
Material의 Fresnel 값을 가지는 Texture는 아직 보지 못했기에 따로 Material마다 지정해주거나 Material에 따라 제작할 필요가 있다.

지금 사용하고 있는 Material마다 지정해주는 방식은 한 Material에서 다양한 재질을 표현할 수 없다는 문제가 있다.
예를 들어, 하나의 Mesh에서 한 부분은 천 재질이고, 다른 부분은 가죽 재질일 때, Roughness 값은 다르게 줄 수 있지만 Fresnel 값은 동일하게 줄 수 밖에 없다. 금속의 경우, Fresnel 값이 매우 큰 것이 다른 재질과의 차이점인데 하나의 Material에서 그것을 표현할 수 없다는 것이 문제가 된다.
하지만 이와 별개로 Material이라는 표현 자체가 재질을 의미하는 만큼, Material 하나에 하나의 재질만 표현해야 하는 것이 아닌가 같은 것도 고려해봐야 할 것 같다.
일단 지금은 Fresnel 값을 Material마다 직접 지정하고 있다.

<img src="https://user-images.githubusercontent.com/21697638/117751361-edfeee80-b24f-11eb-9d7d-5062e195720e.png" width="70%" height="70%"></img>

<img src="https://user-images.githubusercontent.com/21697638/117751397-fe16ce00-b24f-11eb-9328-6e5f1cd4879c.png" width="70%" height="70%"></img>

아래 사진은 하드코딩 되어 있는 Fresnel 값에 차이를 줘 렌더링 한 결과물이다.
[왼쪽 - 0.1, 오른쪽 - 0.9]

SpotLight 생성 시에 SpotLight의 각도를 SpotPower로 조절해주고 있었으나, LightDirection이 normalize 되지 않은 채로 들어가 의도하지 않은 결과가 나오는 점을 수정.

<img src="https://user-images.githubusercontent.com/21697638/117754539-7b910d00-b255-11eb-9875-7b5450ee4364.png" width="70%" height="70%"></img>

3ds Max에서 지금 필요한 모든 정보들을 얻기 위해선 다음과 같은 처리가 필요하다.

1. UnwrapUVW를 하여 UV를 생성
2. TurnToPoly를 하여 모든 Polygon Size를 3 이하로 줄여, 수동 Triangulate (Export 옵션에서 선택하는 Triangulate 만으로는 Binormal과 Tangent가 제대로 생성되지 않았다)
3. Split per-Vertex normals, Tangents and Binormals, Triangulate 에 체크
4. Units은 Automatic, Axis는 Y-up
5. Import 할 때, Position, Normal, Tangent는 YZ Swap, Z 반전
6. Import 할 때, UV는 V 반전

<img src="https://user-images.githubusercontent.com/21697638/117795033-c1b39400-b288-11eb-8eba-6b0d1f3a59c3.png" width="70%" height="70%"></img>

다른 Object를 Humanoid Object의 R Hand Bone까지 옮기는 것을 구현했다.
여기서 옮겨진 Object가 YZ평면으로 반전된 곳에 위치하는 문제가 있었는데
이것은 AnimClip을 import 할 때, 해당 평면에 대해 반전되서 들어오는 것을 별다른 처리 없이 그대로 사용한 뒤,
VS에서 모든 Vertex에 대해 일괄적으로 position.x *= -1를 해주는 것으로 마무리했기 때문이다. (이렇게 해야 의도한 대로 좌우가 맞게 출력되므로)
지금은 옮기려는 오브젝트에 평면대칭행렬을 곱해서 해결했지만 위에서 말한 AnimClip 행렬들에 평면대칭행렬을 곱해서 VS에서 추가적인 계산을 할 필요 없도록 수정해야 할 것

### 2021.05.12

<img src="https://user-images.githubusercontent.com/21697638/117913228-fd4b6e00-b31b-11eb-954c-3f05813e5e4a.gif" width="70%" height="70%"></img>

손 위치에 Object를 옮기는 것에 성공했다.
Object 클래스에서 부모에 상대적인 로컬변환행렬과 절대값인 월드변환행렬을 받을 수 있도록 구현.
에셋이 좌우반전되는 문제는 쉽게 해결할 수 있는 부분이 아닌 것 같아 일단 묻어두고 이후에 수정.

<img src="https://user-images.githubusercontent.com/21697638/117935310-8c697d80-b33e-11eb-8b7a-22d406e4309b.png" width="70%" height="70%"></img>

<img src="https://user-images.githubusercontent.com/21697638/117935367-9be8c680-b33e-11eb-87e7-cf6485aefefc.png" width="70%" height="70%"></img>

항상 도움받고 있다.
애니메이션 정보를 좌표계에 문제가 없이 가져오는데 성공했다.
이제 전후좌우 전부 맞다. AnimatedObject와 일반 Obecjt 간에 CCW 차이 또한 없다.

    // In AnimClip Parser
    FbxAxisSystem d3dAxisSystem(FbxAxisSystem::EUpVector::eYAxis, FbxAxisSystem::EFrontVector::eParityOdd, FbxAxisSystem::ECoordSystem::eLeftHanded);
    d3dAxisSystem.DeepConvertScene(lScene);

	// In Main Program Importer
	// 이 부분은 Mesh Parser에서 처리하고 넘어오는게 좋을 것 같다.
	temp.m_xmf3Pos = vecCP[v.ctrlPointIndex].position;
	swap(temp.m_xmf3Pos.y, temp.m_xmf3Pos.z);
	temp.m_xmf3Pos.z *= -1;
	temp.m_xmf3Pos.x *= -1;
	temp.m_xmf3Normal = v.normal;
	swap(temp.m_xmf3Normal.y, temp.m_xmf3Normal.z);
	temp.m_xmf3Normal.z *= -1;
	temp.m_xmf3Normal.x *= -1;
	temp.m_xmf3Tangent = v.tangent;
	swap(temp.m_xmf3Tangent.y, temp.m_xmf3Tangent.z);
	temp.m_xmf3Tangent.z *= -1;
	temp.m_xmf3Tangent.x *= -1;
	temp.m_xmf2UV = v.uv;
	temp.m_xmf2UV.y *= -1;

이렇게 하면 에셋 제작 시에 의도한 모습으로 애니메이션이 재생되고, 추가적인 처리를 해줄 필요도 없다.

### 2021.05.13

Mixamo에서 애니메이션을 다운로드 하여 Parse하고 적용하였다.
Mixamo 사용법은 다음과 같다.

1. Biped를 남기고 모두 제거한 뒤, FBX로 Export하여 Mixamo에 업로드한다.
2. 원하는 애니메이션 클립을 선택하고 Without Skin 옵션을 선택한 뒤, 다운로드 한다.
3. Skin이 적용된 Mesh와 Biped가 같이 있는 Max 파일에서 방금 다운로드한 FBX 파일을 Import 한다. (이때, Import 설정은 https://m.blog.naver.com/bluegod731/221167655192 를 참고)
4. 애니메이션이 적용된 Max파일을 그대로 Export하여 AnimParser에서 처리하면 그대로 사용할 수 있다.

해상도를 1920*1080으로 변경하였다.
Cascade Shadow Map 구현에 있어서 버그를 찾았으므로 이를 수정하는 것을 최우선 목표로 한다.

### 2021.05.14

<img src="https://user-images.githubusercontent.com/21697638/118232331-573e6600-b4cb-11eb-9733-548b1a0b22e7.png" width="70%" height="70%"></img>

창의 크기를 1920*1080으로 늘렸다.
Cascade Shadow Map 기능에 버그가 있어 수정하였다. 이제 가끔씩 보였던 자잘한 그림자들이 보이지 않게 됐다.

-----------

### 21.05.17 - 21.05.23

#### 이번주 일정

* Humanoid의 이동 State들을 구현
* 보행기반 Animation Blend 구현

#### 2주 목표

* Animation Blend
* IK
* 마우스로 시선을 돌리면 몸이 살짝 틀어지고, 머리가 해당 방향을 보고, 우클릭을 누르면 조준을 하고 , 그 상태에서 하체는 이동을 따로 하는 등

### 2021.05.17

Animation Blend를 구현하기 위해 우선 HumanoidState부터 변경하였다.
Humanoid_WalkForward에서 Humanoid_Moving으로 구조를 바꾸고, State 안에서 KeyInput을 받게 하였다.
State는 이제 vecAnimClipNameList를 반환한다.
AnimationCtrl은 미리 Blend를 위해 하나의 ClipName이 아닌 ClipName이 담긴 vector를 인자로 받게 변경하였다.

<img src="https://user-images.githubusercontent.com/21697638/118416437-a9a99d80-b6ea-11eb-893e-233858dbb3d0.gif" width="70%" height="70%"></img>

Animation Controller 부분에서 pair<string, float>를 받게 변경하고 행렬에 가중치를 곱한 결과물을 모두 합해서 반환시켰다.

<img src="https://user-images.githubusercontent.com/21697638/118465773-2e250c00-b73d-11eb-958f-8a06607587d2.gif" width="70%" height="70%"></img>

### 2021.05.19

<img src="https://user-images.githubusercontent.com/21697638/118802238-dde4b000-b8dc-11eb-9d3f-460e5000a6fd.gif" width="70%" height="70%"></img>

AnimationController 클래스의 SetMatrix()와 GetBoneMatrix() 함수를 MakeAnimationTransform(), GetLatestToWorldTransfromOfSpecificBone(), SetAnimationTransform() 함수로 대체하였다.
이제 자식 오브젝트는 부모 오브젝트의 BoneAnimationTransform을 구하기 위해 부모가 어떤 AnimClip들을 사용하는지 몰라도 된다. 다만, 기존의 AnimationController를 전역에 선언하고 모든 AnimatedObject가 같이 사용했었는데 이 부분은 변경해야 한다.

Idle AnimClip이 순간적으로 매우 빠르게 재생되는 문제가 있었는데, 이는 Blend 대상인 Clip들의 시간을 모두 평균화 해주면서 생긴 문제였으며
Idle / Walk, Run 등으로 따로 시간을 맞춰주어 Blend하는게 옳은 방법인 것 같다.
(예를 들어, Walk와 Run은 서로 시간을 평균내서 맞춰줘야 하지만 Idle은 다른 범주의 Clip이므로)

Moving State에서 Weight값을 정해주는 부분을 수정하였다.

발이 겹치거나, 동작이 안 맞거나 하는 부분이 있다.
지금은 Object의 time 하나로 모든 것을 계산하고 있는데 발이 떠있는 상태 등을 확인해야 하므로 이동하기 시작했을 때의 time을 시작부분으로 정해주는 것도 좋을 것 같다.

### 2021.05.20

<img src="https://user-images.githubusercontent.com/21697638/118926050-a62c4580-b97a-11eb-82b0-9850abe6b5d0.gif" width="70%" height="70%"></img>

Backward + Strafe좌우 를 매끄럽게 만들기 위해 Strafe좌우Backward를 만들고,
앞뒤 이동이 반전되는 순간에 방향이 불연속적으로 바뀌는 것을 막기 위해 좌우 이동을 앞이나 뒤 방향을 향하지 않고 좌, 우로만 향하는 클립을 추가하였다.
따라서 지금 사용되고 있는 애니메이션은 다음과 같다.

	Humanoid_Idle.mac
	Humanoid_WalkingForward.mac
	Humanoid_WalkingLeftStrafe.mac
	Humanoid_WalkingLeftStrafeForward.mac
	Humanoid_WalkingLeftStrafeBack.mac
	Humanoid_WalkingRightStrafe.mac
	Humanoid_WalkingRightStrafeForward.mac
	Humanoid_WalkingRightStrafeBack.mac
	Humanoid_WalkingBackward.mac

Blend가 되고 있는 도중에 캐릭터가 얇아지는 문제가 있다. (앞오른쪽 등)
차이가 크게 나지 않는 클립을 섞어주어야 위의 문제가 적어질 것 같다. 지금은 앞으로 이동하는 클립과, 옆으로 이동하는 클립 간의 차이가 커서 생기는 것 같다.

지금 있는 애니메이션은 크게 Idle과 Walk로 볼 수 있다.
Idle에 추가로 Walk를 섞어주는 거라고 생각해야 한다.
Walk 종류들끼리는 서로 Clip Length가 다르기 때문에 이를 맞춰줘야 한다. 또한, Move State에서 별개의 시간을 갖게 하여 이동 애니메이션이 재생될 때, 해당 시간을 가지고 Blend를 해야 한다.
지금은 단순하게 수작업으로 모든 Clip의 Length를 Max에서 수정해줬지만 이것은 에셋을 만드는 디자이너의 의도와 다른 모션을 재생하게 하므로 문제가 있다.

### 2021.05.21

Animation Blend

- 같은 종류끼리는 길이를 맞춰야 함
- Idle은 기본 모션
- 부위에 따라 State를 달리 줄 때, Blend는?

-----------

### 21.05.24 - 21.05.30

#### 이번주 일정

* BaseState, SubState, Action 구조를 구현

#### 2주 목표

* Animation Blend
* IK

### 2021.05.24

구조 설계는 다음과 같다.

	State, SubState, Action

	- State: 키입력 처리, 기본적인 Animation 출력
	- SubState: 특정 변수 조정, State Animation을 Overwrite
	- Action: 상위 Animation을 Overwrite

	Mask Struct도 있어야 함.
	BoneMask::eUpperBody
	BoneMask::eLowerBody
	기본적으론 Bone의 Idx를 갖고 있는 배열
	여기선 Pair로 처리해야 하나?
	Mask가 가져야 할 정보: 몇 번 Bone에 얼마의 값. (스텐실 처럼)

	애니메이션은 BaseState에서 먼저 애니메이션을 계산.	m_AnimCtrl->CalcAnimTransformFromBaseState(vecPairClipWeight, baseStateTime);
	그 다음, SubState에서 계산된 애니메이션에 Blend.	m_AnimCtrl->BlendToPrevAnimTransform(vecPairClipWeight, time, Mask);
	추가로 Action이 있다면 위의 결과값에 추가로 Blend.	m_AnimCtrl->BlendToPrevAnimTransform(vecPairClipWeight, time, Mask);

	State는 한 순간에 단 하나만 존재.
	State간 전환은 Object의 상태를 보고 State에서 판단.	if(IsPossibleToChangeStatdTo(strStateName)) ChangeStateTo(strStateName);
	SubState의 추가 및 시간 갱신은 State에서 판단.			if(IsPossibleToAddSubState(strSubStateName))AddSubState(strSubStateName);
	Action의 추가 및 시간 갱신은 State에서 판단.			if(IsPossibleToAddAction(strActionName))	AddAction(strActionName);

	SubState는 여러 종류가 한 순간에 존재 할 수 있음.
	하지만 이미 존재하는 SubState가 추가로 존재할 수는 없음.
	이미 존재하는 SubState를 추가할 때에는 해당 SubState의 시간을 갱신함. 또는 아무 것도 하지 않음. (애초에 SubState는 유지되는 하위 State니까)

	Action 또한 SubState와 같음.

	HumanoidObject는 다음과 같은 변수를 가짐.
	m_CurrState, m_vecCurrSubState, m_vecCurrAction;

	State->GetPairClipWeight()로 Clip이랑 Weight Pair를 넘기지 말고 그냥 Update()에서 처리해버리거나 Render()에서 처리해버리거나 할 것.
	그래야 SubState, Action에서도 일관성 있게 처리할 수 있음.

### 2021.05.25

BaseState와 SubState를 간략하게 구현.
BoneMask를 간략하게 구현.
AnimationController는 기능을 분할하여 전역에 존재할 AnimationUploader와 namespace AnimationCalculator 로 분리.

<img src="https://user-images.githubusercontent.com/21697638/119498880-1256e100-bda1-11eb-844e-5545aa1cbce4.gif" width="70%" height="70%"></img>

Mask 자체는 문제 없이 작동하는 것 같으나 이렇게 나오는 것이 옳게 계산된 것인지는 잘 모르겠다.

<img src="https://user-images.githubusercontent.com/21697638/119506184-a4161c80-bda8-11eb-913d-d86a4f6292cb.gif" width="70%" height="70%"></img>

### 2021.05.27

<img src="https://user-images.githubusercontent.com/21697638/119802372-73a1c000-bf19-11eb-9509-9e97a7cddcad.gif" width="70%" height="70%"></img>

<img src="https://user-images.githubusercontent.com/21697638/119802886-e874fa00-bf19-11eb-9353-08aa1a05546b.png" width="70%" height="70%"></img>

AnimCalc::Blend() 를 수정했다.
그동안 Blend 결과물이 이렇게 나오는게 맞는지 잘 모르는 상태였는데 이번 결과물을 보고 의도와 다른 것을 확인했다.

조준을 하고 이동을 하면 상체도 골반에 따라 흔들려야 한다.
근데 지금은 Animation Clip을 보간해주는 것뿐이라 골반 흔들림 등에 영향을 받지 않는다.
이동 애니메이션에 의해 골반이 이동하면 나머지 Bone들도 영향을 받아야 한다.
Bone에 영향을 주는 식으로 해야 한다. 결과물을 보간하는 것만으론 의도한 결과물을 낼 수 없다.

-----------

### 21.05.31 - 21.06.06

#### 이번주 일정

* Blend를 다른 방식으로 구현
* Parser에서 toParent와 local Transfrom 행렬을 Export/Import 할 수 있게 구현

#### 2주 목표

* Animation Blend

### 2021.05.31

기존 Blend 방식으론 기획 의도대로 작동하지 않음을 확인.
수정을 위해 Blend 방식을 변경.
각 Clip 별로 계산이 완료된 행렬들끼리 보간하는 것이 아니라 각 Clip들의 Local Transfrom끼리 보간을 하고 toWorld를 실시간으로 계산하는 방식으로 변경해야 함.

toDressposeInv와 toWorld * nKey를 Clip에 저장하던 방식에서
toDressposeInv, toParent, local Transform * nKey, parentIdx를 Clip에 저장하도록 변경하였다.
toDressposeInv, toParent, parentIdx는 Bone Hierarchy Info로 따로 저장해야 하고
local Transform만 Clip이 가지는게 맞지만 우선은 빠른 구현을 위해 Clip에 같이 담았다.

본 프로젝트에서 import 하는 부분까지 문제 없이 구현하였고 겸사겸사 Parser의 세부사항을 수정하였다.

Animation Blend에 대해 아는 내용이 적어 Unity를 참고하였다.
Unity에서는 같은 Layer 내에서 State와 State 사이의 Transition이 있고, 이 때 1차적으로 Animation Blend가 된다.
또한, Layer를 추가하여 동시에 여러 State를 적용할 수 있고, 이 때도 Layer끼리 Blend가 된다.
이 때 필요한 weight와 mask 정보는 Layer가 갖는다.

구현하려면 Transition 동안의 State와 Blend를 처리할 방법과 Layer Blend를 처리할 방법을 만들어야 한다.

-----------

### 21.06.07 - 21.06.13

#### 이번주 일정

* 일주일 휴식

-----------

### 21.06.14 - 21.06.20

#### 이번주 일정

* Animation Blend를 마저 끝내고 생각

#### 2주 목표

* Animation Blend

### 2021.06.14

<img src="https://user-images.githubusercontent.com/21697638/121862792-22296b80-cd36-11eb-8f99-7245e43b4419.gif" width="70%" height="70%"></img>

Layer 별로 BoneMask를 가지고 Blend하는데 성공.
Layer 내에서 State 전환을 구현하면 될 듯.
BoneMask를 어떻게 잘 조절해야 할 것 같다.

-----------

### 21.06.21 - 21.06.27

#### 이번주 일정

* Animation Blend

### 2021.06.21

막연하게 'Animation Blend' 라고 해두니 이게 끝인지 아닌지 감이 안 잡힌다.
따라서 다음의 기능들을 구현하는 것으로 정정한다.

1. Move Layer는 Default State에서 보행이동기반 Animation Blend. (속도에 따라)
2. Action Layer는 State들 간에 Transition을 구현. (Transition 중간에 보간)

<img src="https://user-images.githubusercontent.com/21697638/122705189-2e5e7d00-d290-11eb-9f99-94095959383f.gif" width="70%" height="70%"></img>

Move Layer에서 '앞으로', '오른쪽 앞으로' 클립 blend
Action Layer에서 '조준' 클립 Blend

이젠 얇아지거나 하는 문제가 없다.

### 2021.06.22

<img src="https://user-images.githubusercontent.com/21697638/122872786-e9a81400-d36b-11eb-8a9a-42d9901fbedc.gif" width="70%" height="70%"></img>

	구현사항
	- Layer Blend(weight를 기반으로 하는)
	- State 전환에 따른 AnimClip 변화
	- State Transition 도중의 부드러운 Blend
	- Movement Layer의 보행 이동 기반 Blend
	
	개발 도중 겪은 내용들
	- Blend를 하지 않는 단순 Animation 재생이라면 시간별 toWorld와 toDressposeInv만 가져도 계산할 수 있다.
	- 이미 계산된 행렬을 가지고 계산하는 것은 모델이 얇아지거나 할 수 있다. (이유까지는 아직 잘 모르겠다)
	- Blend를 목표로 한다면 시간별 local(특히 rotation 값을 갖고 있는 quaternion), BoneHierarchy의 toDressposeInv, toParent, parentIdx 등이 필요하다.
	- Quaternion은 Blend 계산에 적합하다.

### 2021.06.24

ComputeShader를 사용하여 Blur와 DoF, HDR, Bloom 정도를 목표로 하고 있다. 저걸로 구현하는 것이 맞는지는 하면서 알아가도록 하자.
새로 브랜치를 생성하고 UAV와 SRV를 둘 다 만들어준 Texture 자원을 생성했다.

    StructType A; 로 생성하는 것과
    StructType A = {}; 로 생성하는 것이 차이가 있었다.
    UAV를 생성할 때, uavDesc 구조체를 인자로 전달해야 하는데 이 때 문제가 있었다.

### 2021.06.25

ComputePipelineStateObject를 작성했다.
ComputeRootSignature를 작성했다.
Resource Barrier랑 Texture의 SRV, UAV(UAV는 SRV를 저장하던 Heap에 그대로 저장했다)생성하는 부분을 작성했다.

ComputePSO를 적용하고, RootSignature도 Set 해준 상태에서 적용이 되는 것을 확인했다.

Thread Index 값은 Group ID / Group Thread ID / Dispatch Thread ID 이렇게 셋이 있다.

만약, 한 스레드 그룹에서 256개의 텍셀을 처리할 예정이고 dispatch 할 때, (width / 256, height, 1) 과 같이 넘긴다면 가로로 width / 256번, 세로로 height번 실행된다.

이 때, 만약 CS에서 numThreads가 [numthreads(16, 16, 1)] 이고 Index 접근을 .xy 로 하게 한다면

<img src="https://user-images.githubusercontent.com/21697638/123380380-54568b00-d5ca-11eb-89f5-509847555cce.png" width="70%" height="70%"></img>

위와 같이 x방향 16칸, y방향 16칸으로 256개의 thread를 사용하게 된다.
근데 dispatch는 가로로 8번, 세로로 1080번을 했으므로 화면의 왼쪽 부분(가로 128, 세로 1080 픽셀)만 효과를 받게 된다.

dispatch를 가로로 1920 / 256번, 세로로 1080번을 하겠다는건 가로로 길게 256픽셀씩 8번, 세로로 1080번을 하겠다는 의미이므로
모든 화면에 적용하기 위해서는 스레드 그룹의 범위를 [16, 16, 1]이 아니라 [256, 1, 1]로 해야 의도한 대로 결과가 나온다.

<img src="https://user-images.githubusercontent.com/21697638/123381323-8c120280-d5cb-11eb-8a20-4946ad8c75ad.png" width="70%" height="70%"></img>

(G버퍼의 Color 텍스처.rgb에 (1.7f, 0.3f, 0.3f)를 곱해준 결과물)

-----------

### 21.06.28 - 21.07.04

#### 이번주 일정

* Post Process 준비
* Blur 구현

#### 2주 목표

* Blur 효과 구현
* Depth Of Field 효과 구현(다만 상황에 따라 HDR 효과로 변경될 수 있음)

### 주요 목표

- Blur, HDR
- DoF, Bloom
- Particle
- Collision Check
- IK
- Extra Bone

Blur와 HDR을 각각 1주, DoF와 Bloom을 각각 1주씩 잡고 최대 일정을 4~6주로 계산.
Particle을 2~4주로 계산.
Collision Check를 8주로 계산하면 큰 일정들을 쳐내는데 최대 4달 반이 소요될 것.
11월 중순쯤이면 뭔가 나와있어야 한다.

### 2021.06.29

<img src="https://user-images.githubusercontent.com/21697638/123766527-bb917980-d901-11eb-9b51-8f87e195089e.png" width="70%" height="70%"></img>

<img src="https://user-images.githubusercontent.com/21697638/123766638-d2d06700-d901-11eb-99c7-1a7d28871e12.png" width="70%" height="70%"></img>

1. 그동안 후면버퍼(rtv와 dsv)를 OMSet하는 줄을 기준으로 RenderPass1()과 RenderPass2()로 나누던 것을 하나의 Render() 함수로 합치고, Render()의 인자로 후면버퍼 핸들을 전해줘서 Render() 함수 내부에서 OMSet으로 후면버퍼를 지정할 수 있게 변경.
2. 렌더타겟용으로 Texture를 두 개 더 만들고(각각 정사이즈 Screen Texture와 1/16사이즈 SmallScreen Texture), CS에서 접근하기 위해 UAV로도 묶은 1/16사이즈 Blur_Vertical, Blur_Horizontal Texture를 생성.
3. G-Buffer의 내용을 합친 것과 Light 결과물을 Screen Texture에 담고 해당 Texture를 1/16사이즈의 평면 메쉬에 입히고 좌상단 1/16공간을 제외한 나머지는 검은 전체 화면을 SmallScreen에 렌더링.
4. 렌더링하면서 Texture 크기를 넘어간 부분(원래 검은 부분)은 전부 잘리고 원래 G-Buffer와 Lighting 결과물만 1/16사이즈 Texture에 저장됨.
5. 작은 해상도의 Texture를 대상으로 수직, 수평 방향으로 Dispatch 하게 작성.

여기까지 진행함.
처음 Horizontal CS에서 SampleLevel()을 가지고 직접 Screen Texture에서 읽어오는 것도 되지 않을까 싶음.

### 2021.06.30

<img src="https://user-images.githubusercontent.com/21697638/123892914-6784a480-d996-11eb-9386-6713c3afc65e.png" width="70%" height="70%"></img>

Blur가 된다.
가중치는 FunctionTest Project에서 구해서 직접 넣어줬다.
int gBlurRadius 를 전역에 두면 모든 픽셀이 검게 나오는 문제가 있었는데 이유는 모르겠다.

-----------

### 21.07.05 - 21.07.11

#### 이번주 일정

* HDR 구현

#### 2주 목표

* Post Process를 HDR, Bloom까지 마무리 짓기
* Multi Thread Rendering 공부하기

### 주요 목표

- MultiThread Rendering
- ~~Blur,~~ HDR
- ~~DoF,~~ Bloom
- ~~-Particle~~
- ~~-Collision Check~~
- ~~-IK~~
- ~~-Extra Bone~~

### 2021.07.05

<img src="https://user-images.githubusercontent.com/21697638/124487449-b2187d80-dde9-11eb-8857-4af390f3c9a2.png"></img>

왼쪽부터 원본, Blur, HDR 결과물

주말에 다렉 작업을 하고 있는 분들과 진로 얘기를 짧게 했고 일단 후처리 구현이 끝낸 뒤에 멀티스레드 렌더링 쪽을 공부해보는게 좋을 것 같다고 결론이 났다.
면접에서도 멀티스레드 렌더링 관련된 질문을 물어본다고 하니 그 쪽을 준비해두는게 좋을 것 같다.
클라이언트 프로그래머로 취직을 한다고 해도 그 다음엔 무엇을 할 것인지 진로를 세세하게 그려봐야 한다.

이번 HDR 같은 경우엔 책의 내용을 그대로 따라 적어서 구현했다. 중요한 부분이 이해가 되지 않았는데 이런 식으로 구현만 하고 넘어가는 것에 의미가 있는지 모르겠다. 원리를 이해하고, 내가 만들고 싶은 효과를 기획하고, 어떻게 만들지 구상해서 만들 수 있어야 하지 않을까.

### 2021.07.06

<img src="https://user-images.githubusercontent.com/21697638/124603086-873f2f80-dea4-11eb-9b55-cd30ecf8f431.png" width="70%" height="70%"></img>

현재 픽셀의 휘도가 평균 휘도 + Bloom threshold보다 높은 걸 텍스처로 만들어서 Blur 효과를 주면 Bloom 효과가 된다.
건드릴 수 있는 수치는 Bloom threshold랑 fBloomScale 이렇게 둘.

HDR 효과의 코드 동작 원리를 모른 채로 구현에 성공한 줄 알았는데 역시 아니었음.
Bloom 효과는 구현했으니 평균 휘도 구하는 코드를 책을 참고해서 수정해야 함.
경우에 따라서 직접 작성해보는 것이 더 나을 것 같기도.
일단 오늘은 여기까지.

### 2021.07.07

10시부터 4시까지 평균 휘도 구하는 코드를 이해하고 내 프로젝트에 맞게 고쳤다.
함수 이름과 인자 이름을 보고 헷갈리지 않아야 좋은 이름임을 느꼈다.

첫 번째 패스[numthreads(1024,1,1)]에서는 1/16사이즈로 다운스케일, 스레드 그룹의 공유 메모리에 휘도 저장, 공유 메모리에 저장된 휘도들을 다운스케일해서 구한 평균 휘도를 gfAvgLum[Gid.y]에 저장.
(공유 메모리가 float 1024 크기의 배열이고 이걸 4로 만들고, 1로 만들고 하는 것 때문에 함수 이름이 1024to4 랑 4to1이었다)
스레드 그룹의 스레드가 x축으로 1024개 있기 때문에 한 스레드 그룹으로 1/4 Width를 처리할 수 있다.
1/4 Height는 270이기 때문에 Dispatch는 (1, 270, 1)과 같이 인자를 넘겨준다.
이렇게 하면 gfAvgLum[270]까지 모든 가로줄의 평균 휘도가 구해진다.

두 번째 패스[numthreads(1024, 1, 1)]에서는 gfAvgLum[270]을 하나의 평균 휘도로 만든다.
1024->256, 256->64, 64->16, 16->4, 4->1 의 과정을 거쳐 하나의 평균 휘도를 구하면 끝.

아직도 헷갈리는 부분이 너무 많다.

<img src="https://user-images.githubusercontent.com/21697638/124716563-30d2ff00-df3f-11eb-96ec-16d1a6e96cfd.png" width="70%" height="70%"></img>


-----------

### 21.07.12 - 21.07.18

#### 이번주 일정

* Multi Thread Rendering

### 2주 목표

* Multi Thread Rendering

백신 1차 접종 맞고 푹 쉬었다.

### 2021.07.13

멀티스레드 사용법을 먼저 리마인드. 예전에 들었던 멀티스레드 프로그래밍 강의 내용을 어떤 분이 정리해두신게 있어서 참고했다.

### 2021.07.14

[Tips and experience of DX12 Engine development . (slideshare.net)](https://www.slideshare.net/dgtman/tips-and-experience-of-dx12-engine-development)

멀티스레드를 사용할만한 부분은

- G-buffer 생성 시에 오브젝트들을 스레드 별로 나눠 그리기
- 그림자 깊이맵을 스레드 별로 나눠 그리기

정도가 있을 것 같다.

미리 CmdList를 여러 개 생성해서 들고 있다가 필요한 부분에서 새 스레드를 생성해서 명령을 넣는 방법을 먼저 해보고... 다른 방법도 좀 찾아보고... 해봐야 할 것 같음.

### 2021.07.15

<img src="https://user-images.githubusercontent.com/21697638/125733034-af46b913-bb07-446d-8a42-59c32ca1dc64.png" width="70%" height="70%"></img>

<img src="https://user-images.githubusercontent.com/21697638/125733135-cfa488ca-5875-43f8-8e6c-4b2331707c3e.png" width="70%" height="70%"></img>

AnimUploader를 전역에 하나 만들던 것에서 각 AnimatedObject 별로 하나씩 갖게 하였다. (모든 객체가 각자 애니메이션 데이터를 업로드 할 수 있도록)
사람 오브젝트에게 간단한 AI를 만들어서 무작위 방향으로 움직이도록 만들고, 721개를 (1개는 플레이어가 조종) 생성했다.

프레임 한 장 그릴 때 걸리는 시간을 줄이는게 목표.

절두체 컬링이나 인스턴싱 같은걸 쓰지 않고 오로지 멀티스레드 렌더링만으로 프레임을 줄여보자.

-----------

### 21.11.08 - 21.11.14

#### 이번주 일정

* Collision Detection

멀티스레드 렌더링을 시도해보면 나쁠건 없어 보이지만 일단 초기 목표였던 충돌검사와 충돌처리를 먼저 간단하게 끝내고 나서 다시 생각해보는게 어떨까 싶어 기존 일정이었던 멀티스레드 렌더링을 치웠다.

물리엔진을 만들 것도 아니고, Physx를 붙일 것도 아니기 때문에 매우 간단한 충돌 검사와 이후 보정 등만 해야 한다.
일단 만들어보고 성능적인 문제가 크면 그 때 공간분할이든 뭐든 추가하고 그게 아니라면 따로 추가하지 않는다.
(60프레임 방어를 못할 경우)

-----------

### 21.11.14 - 21.11.21

#### 이번주 일정

* Collision Detection

### 2021.11.15

BoundingOrientedBox 를 사용해서 Intersects()로 검사하는 테스트 코드를 작성했다.
깃허브 데스크탑의 git: 'remote-https' is not a git command. See 'git --help'. 라는 오류 때문에 갑자기 push가 되지 않는다.

### 2021.11.16

시스템 환경변수 문제였던 것 같아 예전에 추가했던 환경변수들을 정리하고, \AppData\Local\GitHubDesktop 와 \AppData\Roaming\GitHub Desktop 를 삭제한 뒤, 재설치했다.
모든 git과 관련된 프로그램을 삭제하고, 파이참, 아나콘다 등도 삭제했다.
다시 publish branch와 push가 된다.

-----------

### 21.11.22 - 21.11.28

#### 이번주 일정

* Component
* Collision Detection

### 2021.11.22

기존 코드에 깔끔하게 Collider를 추가할 방법이 떠오르지 않아 Component 방식을 다시 한 번 시도해보기로 했다.
Transform, MeshRenderer, SkinnedMeshRenderer, Collider(또는 BoxCollider, SphereCollider로 나누거나) Component를 작성하고 테스트 할 예정.
Component Class를 작성하고 Transform과 MeshRenderer Class가 Component를 상속받도록 작성하였다.
MeshRenderer는 같은 Object가 가지고 있는 Transform에 접근할 수 있어야 하므로 Object 내부에서 Component를 이름으로 찾을 수 있는 함수를 추가하였다.

### 2021.11.23

코드 리팩토링.

- Component를 추가하기 위해, Object에서 불필요한 부분을 모두 들어냈다.
- Mesh를 상속받지 않고 별개로 존재하던 DebugWindowMesh(디버깅용 작은 평면 메쉬로 쓰다가 이후에 RTT용도로도 사용) 클래스를 정리하고 DebugWindowObject 또한 삭제했다. 이는 Screen을 띄우는 용도로만 사용하는 개별 클래스로 따로 변경했다.
- Animation 파트를 전부 들어내고 AnimatedObject 또한 잠시 주석처리하였다.
- MeshRenderer, Transform, Controller Component를 작성하고 정상 작동 확인했다.

Collider Component를 내일 추가하고 정상작동 확인하면 SkinnedMeshRenderer와 Rigidbody 역할을 할 Component를 작성하도록 하자.

### 2021.11.24

Collider Component 작성.

충돌 관련 처리는 순서가 중요하다.
한 프레임 내에서 CheckCollision(), SolveConstraint(), Update() 순서로 진행하자.
Scene::CheckCollision() 에선 모든 Object에 대해 다른 Object와의 충돌 여부를 검사한다.

    // Scene
    for( i ; i < size ; i++ )
	    for( j = i + 1 ; j < size ; j++ )
		    [i]->CheckCollision([j]);
		    
	// Object
	// 따로 충돌여부를 Scene 단까지 반환하지 않는다.
	for( i ; i < myCollider.size ; i++ )
		for( j ; j < otherObjectCollider.size ; j++ )
			[i]->CheckIntersect([j]);
	
	// ColliderComponent
	if(collide) {
		m_vecpIntersectedCollider.push_back(other);
		return true;
	}

ColliderComponent에서 충돌한다고 판정나면 Component 내에 저장한다. (Object 내에 저장하지 않기 위해)
A Object의 Collider가 B Object의 여러 Collider와 Intersect 할 수 있고, 감지된 Collider가 B Object의 '벽, 바닥 감지용 Collider'인지, 'Hitbox Collider'인지 모르기 때문에 Collider를 전부 저장한다.

SolveConstraint() 단에서 해당 Collider가 어떤 용도인지 체크하고, 맞는 처리를 한다.

Update()는 충돌처리 외의 부분을 하면 될 듯.

### 2021.11.25

한 Object 내에 지면판정용 Collider와 피격판정용 Collider를 넣어뒀을 때, CheckCollsion()에서 지금 충돌한 Collider가 지면판정용인지, 피격판정용인지 알 수 없다는 문제가 있다.
유니티에서는 한 gameobject 내에 같은 타입의 Collider(예를 들어 BoxCollider 두 개 이상)를 각자 다른 용도로 사용할 수 없다는 글이 있었고 이를 구현하려면 Child Object를 생성해서 Collider를 넣어주는 식으로 구현해야 한다.

그 외에 InputEvent()에서 인자로 흐른 시간을 받고 있어서 이를 정리했다.

-----------

### 21.11.29 - 21.12.05

#### 이번주 일정

* Object 내에서 원하는 Component를 찾아서 반환시키기

### 2021.11.29

	// 이 방식을 쓰면 한 Object 안에 있는 모든 Component에 대해서
	// 원하는 자료형으로 변환이 되는 Component만 찾아서 반환시킬 수 있다.
	template<typename t>
	vector<t*> FindComponents() {
		vector<t*> result;

		for (Parent* at : m_vecpParent) {
			t* as = at->GetDerived<t*>();
			if (nullptr != as)
				result.push_back(as);
		}
		return result;
	}

한 Object의 모든 Collider를 찾을 때, 그 Object와 해당 Object의 Child Object들까지 전부 찾아서 모든 Component에 대해 검사한다.
시간이 오래 걸릴 것 같기도 해서 걱정되는데 실제로 돌려보고 프레임 유지가 힘들 것 같다 싶으면 고민해보는 것으로.

Component 기본 클래스를 작성하였다.
유니티의 컴포넌트들이 사용된 모습을 참고하여, Collider, Controller, MeshRenderer, Transform Component들을 간단하게 추가하였다.

-----------

### 21.12.06 - 22.01.09

#### 이번주 일정

* Component 구조로 변경하기

### 2021.12.06

ObjectData.data의 구조를 다음과 같은 방식으로 변경 예정.

	{
		"ObjectName001", "TagName", "ParentName"
		"Transform", "0, 0, 0", "0, 270, 0"
		"SkinnedMeshRenderer", "heavyGuardModel", "heavyGuardMat"
		"BoxCollider", "0, 0.3, 0", "1, 2, 1", "0, 0, 0, 1"
		"Character", "100"
		"Controller", "5"
	}
	{
		"GroundCheck", "CollisionDetector", "ObjectName001"
		"Transform", "0, -0.3, 0", "0, 0, 0"
		"SphereCollider", "0, 0, 0", "1.5"
		"Rigidbody"
	}

### 2021.12.07

FindComponent() 보충 작성

### 2021.12.23

Object::FindComponents()와 Object::FindComponentsInChildren() 함수를 작성하였다.
FindCompnentsInChildren()을 위해 보조 재귀함수도 추가.

SkinnedMeshRenderer, Animator Component의 틀을 간단하게 추가하였다.

Transform Component에서 Local Space 외에도 World Space의 값 또한 제공할 필요가 있다 생각되어 일단 TransformComponent::GetWorldTransform()을 작성하였다.

### 2021.12.27

A, B, C 순서대로 상속 관계일 때

    A* c = new C();
    dynamic_cast<B*>(c)->f();

를 하면 B가 아니라 C의 f()가 호출된다.
위의 A, B, C를 각각 Component, AnimatorComponent, HumanoidAnimatorComponent로 보고 f()를 Update()로 바꿔보면 될 것이다.

### 2022.01.04

Happy New Year~

충돌체 구현을 위해 컴포넌트 구조로 변경하는데 성공했다.

작업한 내용:

	- DebugWindowMesh와 DebugWindowObject, AnimatedObject 같은 class들을 삭제하였다. 급하게 개발하면서 만들어둔 내용을 쳐냈다.
	- Object 내에서 이동, 가속도, 부모의 월드변환행렬 받아오기 등의 일을 하던 함수들 전부 삭제하였다.
	- **Transform, MeshRenderer, SkinnedMeshRenderer, HumanoidController, WeaponController, HumanoidAnimator, InputManager 등의 Component를 작성하고 이를 Object에 추가하여 사용하게 하였다.**
	- 다만, batch 처리를 위해 Object들의 관리는 Non-Animated / Animated로 나눠서 관리한다. (그렇지 않으면 Object 하나 그릴 때마다 Animation 여부에 따라 SetPSO 해줘야 한다)
	- 코드를 일부 정리하였다. (예를 들어, Importer.h에서 불필요한 include header들을 삭제하였다)
	- AnimationUploader class를 삭제하고 SkinnedMeshRenderer Component에 넣어서 Object 별로 다른 애니메이션 클립을 재생할 수 있게됨
	- **State class를 삭제하였다.** 혼자서 개발하다보니 State를 만들고 작성하고 하는건 코스트에 맞지 않다고 생각했다. 앞으로 ~Controller Component와 ~Animator Component에서 해당 class의 역할을 하게 될 것.
	- ObjectData.data 파일을 읽어서 Import를 하는건 중단하였다. 어떤 Object가 어떤 Component를 갖고, 어떤 값을 가지게 되는지 등을 전부 자동화 한다고 해서 작업시간이 크게 단축되거나 할 것 같지 않다. (어쨌든 내가 손으로 작성해야 하므로) 
	- template 방식을 사용하여 FindComponent() 함수를 작성하였다. 이제 m_pObject->FindComponent< TransformComponent >()->GetLookVector() 와 같이 사용할 수 있다.
	- Collider Component를 일부 작성해뒀다. 

<img src="https://user-images.githubusercontent.com/21697638/148066942-3b821378-ec73-4f38-ba3f-aa7d6d9bb3c6.png" width="70%" height="70%"></img>

충돌 검사를 위해 BoxCollider와 SphereCollider Component들을 작성하고 Object에 추가하여 작동까지 확인하였다.

### 2022.01.06

<img src="https://user-images.githubusercontent.com/21697638/148333823-cb4ee025-5563-4acd-9a1d-04c1f4eef50a.jpg" width="70%" height="70%"></img>

작업한 내용:

	- BoxColliderComponent와 SphereColliderComponent, RigidbodyComponent를 작성하였다.
	- Collider들은 localTransform을 가진다. 비교를 위한 BoundingOrientedBox와 BoundingSphere는 Object의 WorldTransform과 ColliderComponent의 LocalTransform 값을 곱해서 만든 최종변환행렬에 의해 Center와 Orientation이 정해진다.
	- 현재 프로그램의 함수 호출 순서는 CheckCollsion(), SolveConstraints(), Update() 순이다.
	- CheckCollision()에서는 Object들마다 서로 충돌하는지 여부를 검사한다. 충돌할 경우, Collider Component 내에 충돌한 Collider의 주소를 저장한다.
	- SolveConstraints()에서는 충돌했을 때, Component별로 추가적인 처리를 한다. 예를 들어, 지금MeshRenderer Component는 충돌한다고 해서 추가적인 처리를 필요로 하지 않는다. Rigidbody Component는 충돌한 Collider에 따라 처리해야 할 내용이 있다.
	- Rigidbody Component는 Update()에서 Transform의 currPosition을 저장하고, 이를 다음 프레임의 SolveConstraints()에서 prevPosition으로 사용한다. 이를 위해 Rigidbody Component는 현재 위치를 변경하는 그 어떤 Component들보다 우선적으로 호출되어야 한다. (예를 들어, HumanoidControllerComponent)
	- Player가 다른 Collider에 충돌하였을 경우, 뚫고 지나가지 않는 동시에 미끄러지는 듯한 효과를 주기 위해 다음과 같은 전략을 사용하였다.
	- Ray 3개를 나란히 쏴서 Collider에 충돌하는 지점을 찾고, CollsionPoint의 개수에 따라 위치를 보정하였다.
	- CollisionPoint가 하나일 경우, CollisionPoint에서 이동방향의 수직으로 일정값만큼 이동하여 보정하였다.
	- CollisionPoint가 둘일 경우, 임의의 CollisionPoint를 복사하여 y값만 달리하여 충돌면을 만들고 노멀벡터를 계산하였다. 이렇게 구한 노멀벡터로 위치를 보정.
	- CollisionPoint가 셋일 경우, CollisionPoint들로 충돌면을 구하고 노멀벡터를 계산하였다. 단, 모서리와 같이 충돌면 두 개에 Ray가 나뉘어 맞는 경우가 있는데 이럴 땐 중앙 Ray가 충돌한 CollisionPoint를 복사하여 CollisionPoint가 두 개일 때와 같이 보정하였다.

Player의 SphereCollider의 크기를 줄여 좀 더 디테일하게 충돌처리를 하고 싶은데, Collider의 크기를 줄이면 충돌체의 모서리나 겹치는 부분에서 충돌체를 뚫는 문제가 발생한다.

<img src="https://user-images.githubusercontent.com/21697638/148335055-a33e41d1-8569-4033-a5c9-21a4402785a6.png" width="70%" height="70%"></img>

-----------

### 22.01.10 - 22.01.16

#### 이번주 일정

* Component 계속 추가하기

### 2022.01.11

기존 카메라의 역할을 CameraComponent로 옮기고, Camera 전용 Object를 생성하여 Player Object의 Child로 넣었다.

작업한 내용:

	- TransformComponent에서 Get~() 함수에 Space::local, Space::world 옵션을 받게 변경하였다.
	- CameraComponent를 작성하고 기존 Camera를 대체하였다.

### 2022.01.12

풀스크린 테스트를 해봤는데 현재 사용하는 모니터는 QHD 해상도고, 프로그램의 해상도는 고정 FHD 해상도여서 커서의 위치 고정 등에 문제가 있다.
간단하게 커서의 위치와 변동값 등으로 회전할 각도 등을 구하고 있었는데 전체화면 상태 등에서도 적용하려면 다른 방식을 사용해야 한다.

마우스 고정 관련된 코드 추가

Object와 InputManagerComponent가 마우스 변동값(float2)를 받도록 수정

HumanoidControllerComponent의 멤버 비정적 데이터들을 생성자에서 초기화리스트로 초기화하도록 수정
또한, 이제 보는 방향에 따라 이동하도록 수정

Collider 외의 Component에서도 충돌 감지나 RayCast 등을 사용할 필요가 있어서, 다른 Component에도 CheckCollision() 함수를 추가하였다.

WeaponControllerComponent에서는 Fire()가 호출되면 다음 CheckCollision()에서 RayCast를 하고, SolveConstraint()에서 충돌한 객체와 충돌점이 있는지 구한다.
지금은 간단하게 충돌한 객체의 종류와 관계없이 해당 오브젝트를 SetActive(false)시키는 것으로 처리.


-----------

### 22.01.17 - 22.01.30

#### 이번주 일정

* Particle & Effect 추가하기

### 2022.01.26

<img src="https://user-images.githubusercontent.com/21697638/151126640-3eeb7ffb-5df3-4d3c-8435-3237923671ed.gif" width="70%" height="70%"></img>

좌클릭을 하여 권총을 쏘고, 맞은 오브젝트에게 피격 처리를 할 수 있게 추가하였다.
권총 사격 시에 총구 화염 이펙트를 추가하였다.
파티클과 이펙트를 사용할 수 있게 작성하였다.

작업한 내용:

	- Collider 외의 Component에서도 RayCast 같은 기능이 필요하다 판단되어, 다른 Component도 CheckCollision() 함수를 갖게 하였다.
	- WeaponControllerComponent는 이제 CheckCollision()에서 RayCast가 필요한 상황이면 RayCast를 하고, 충돌한 가장 가까운 Object의 CollisionComponent를 갖게 하였다.
	- 또한, WCC의 SolveConstraint()에서 충돌한 ColliderComponent가 있을 경우, 해당 Object를 SetActive(false)하게 하였다. (가장 기초적인 처리만 추가하였음)
	- 앞으로 사격 시에 충돌점을 구할 수 있다.
	- Component 기본 클래스는 이제 가상 소멸자를 가진다.
	- ParticleComponent와 ParticleEmitterComponent 클래스를 작성하였다.
	- ParticleComponent는 MeshRendererComponent나 SkinnedMeshRendererComponent와는 별개로 Render를 해야하기 때문에 Mesh를 사용하지 않고 Point Vertex 하나만 GPU로 전달할 수 있게 별도 작성하였다.
	- ParticleComponent는 Render() 함수를 가진다.
	- ParticlePSO를 작성하였다.
	- ParticlePSO는 DepthTest는 하되, Depth를 덮어쓰지는 않는다.
	- DeferrdRendering을 하고 있기 때문에, Screen에 1차적으로 Render를 하고 그 위에 Particle을 Render하였다.
	- ParticlePSO는 가산 혼합 방식으로 작성하였다.
	- Particle Render를 위한 VS, GS, PS를 작성하였다.
	- GS_Particle()은 Point 기본도형을 받아 TriangleStrip형태로 정점 4개, 삼각형 두 개를 반환한다.
	- Particle Pool은 Scene에서 갖고 있는다.
	- ParticleEmitter는 ParticlePool에서 m_bEnabled가 false인 Object를 찾아 자신의 m_vecParticle에 추가하고 자신이 Update()와 Render() 등을 관리한다.
	- MuzzleFlash와 ParticleTest 이미지를 만들고, 알파채널을 추가하여 dds 포맷으로 추가하였다.
	- Mesh.h와 Mesh.cpp의 쓰지 않고 있던 DebugWindowMesh 클래스를 삭제하였다.
	- ModelManager::Render()의 람다 함수 캡쳐모드를 =에서 &로 변경하였다.
	- WCC::Fire() 시에 MuzzleEffect를 Render한다.

-----------

### 22.01.31 - 22.02.06

#### 이번주 일정

* 설연휴 휴식

-----------

### 22.02.07 - 22.02.13

#### 이번주 일정

* TargetBoard Object 만들기

TargetBoard가 Stand, Down 상태일 때, 각각 현재 Bone의 방향에 따라서 Collider를 변환해주어야 한다.
그동안 Collider의 위치와 방향 결정에 있어서 의도와 다르게 작동하던 것을 확인하였다(지금까지는 Collider가 포함된 Object가 회전하는 일이 없어서 발견되지 않음).

### 2022.02.11

추가된 내용:

	- Humanoid Animator Component에서 Aiming Layer의 자세를 보정하기 위해, AdjustRotationQuaternion() 함수를 만들고 Humanoid Bone 중에서 Spine0, 1, 2, 그리고 좌우 Clavicle을 보정했다.
	-  TargetBoard 진행중

-----------

### 22.02.14 - 22.02.27

#### 이번주 일정

* TargetBoard Object 만들기
* 피격 시 파티클 이펙트 생성하기

### 2022.02.14

	- Component class의 생성자에 pObject->AddComponent(this)를 추가하였다. 현재, Component의 추가 순서에 영향을 받는 Component가 몇몇 존재한다.
	- 모든 Object는 이름을 나타낼 string 비정적 데이터 변수를 가지고, Scene 내에서 FindObjectByName(strName)으로 찾을 수 있다. 
	- TargetBoard는 체력을 가지고, 피해를 받아야 하고, 체력이 0 이하가 되면 죽어야 한다. 일정 시간 후에 다시 살아나는 것도 해야함.
	- 그래서 Object의 Enable/Disable과 별개로, HP가 있는 Object들의 HP 관련 수치를 관리할 Character 클래스를 작성. Character는 Component가 아니다.
	- TargetBoard Controller Component는 Component와 Character를 다중상속한다. (이름을 TargetBoard Controller Character Component 같이 지어도 괜찮을 듯)
	- 3ds Max에서 간단한 판자 모델을 만들고, 애니메이션을 적용하여 Export하였다.
	- Humanoid Animation Clip은 제대로 Parse되고, Import 되는데, Bone으로 직접 만든 Hierarchy는 값이 오염되는 일이 있어 bipad를 사용하여 애니메이션을 만들었다.
	- Skinned Mesh와 별개로, Collider 또한, Animator에 영향을 받을 수 있게 변경하였다. 이제 Collider를 Child Object로 두고 Parent의 Bone Transform Matrix를 받지 않고도 한 Object 내에서 Animator가 계산해둔 finalResultAnimationTransform[boneIdx]를 통해 Collider의 위치와 방향을 결정할 수 있다.

<img src="https://user-images.githubusercontent.com/21697638/153813631-40f8710b-efba-4f7e-b631-56fcdcf17d42.gif" width="70%" height="70%"></img>

TargetBoard의 체력은 50, 피격 시에 Damage(100), 다시 살아나는 시간은 3.0s로 설정.

### 2022.02.15

마우스 위아래 조종으로 캐릭터의 팔과 목, 머리 부분을 수정해야 함. (스파인까지)
카메라가 플레이어가 조준하는 방향을 보게 해야 함.
카메라는 지금 Player의 Child로 있고, Head를 Focus 중.
TargetPos랑 Camera를 Head의 Child로 두고, Head의 각도를 조절하는 것도 괜찮을 것 같음.

Object를 Scene의 vector<Object*>에 넣으려면, 밖에서 접근할 방법이 있어야 함.
유니티는 GameObject에 Scene 주소? 를 가지고 있음.
GameObject.Instantiate() 함수로 Scene에 Object 추가 가능.

-----------

### 22.02.28 - 22.03.06

### 2022.03.02

AnimationControllerComponent에서 R Hand의 방향을 조금 꺾어서 조준 방향을 조절.
마우스 고정을 켜고 끌 수 있는 단축키를 추가. (M/N)
배치를 변경함.

-----------

### 22.03.07 - 22.03.13

#### 이번주 일정

* Render Text

### 2022.03.12

[11. Drawing text in DirectX 12 - Braynzar Soft](https://www.braynzarsoft.net/viewtutorial/q16390-11-drawing-text-in-directx-12)를 참고하여 렌더 텍스트를 구현하자.

구현한 내용:

	- TextRendererComponent를 작성하였다.
	- 영어 대소문자, 숫자, 기본적인 문장부호 몇 가지를 지원한다.
	- SetText(), SetSize() 등을 지원한다.
	- Transform Component로 화면에서의 위치를 조절할 수 있다(좌하단(0,0) - 우상단(1920,1080)).
	- Text 출력을 위한 PSO, VS, GS, PS를 작성하였다.
	- 출력할 수 있는 문장의 최대 길이는 80자로 정해두었다.
	- 실시간 Text 수정 및 출력을 활용할 수 있는 script를 작성하였다(EnemyDownCounter).
	- 대상으로 할 Character class를 상속받는 Component를 지닌 Object를 추가할 수 있다.
	- 해당 Character가 죽을 경우, 죽는 그 순간에 count를 1 더한다.
	- 이를 화면 좌상단에 출력한다.
	- 카메라의 위치를 플레이어 캐릭터의 뒤에서 오른쪽 뒤로 조절하고, 바라보는 조준점의 위치를 변경하였다.

<img src="https://user-images.githubusercontent.com/21697638/158320803-58127e1d-029d-4614-b9bb-312f7ef3294a.gif" width="70%" height="70%"></img>

-----------

### 22.03.29 - 22.04.04

#### 이번주 일정

* Unity로 Level Editor 만들기
* 다른 DX12 프로젝트 Shadow 그리기

Unity로 만들 Level Editor가 가져야 할 기능을 정해두고 만들자.

	- FBX 모델을 배치하고, 텍스처를 씌우고

현재 에셋 컨버팅 과정은 다음과 같다.

1. 3ds Max에서 모델을 만들고 FBX 포맷으로 Export한다(필요하다면 리깅까지 한다).
2. 사용할 이미지 파일을 textures.com에서 tif 포맷으로 다운로드 한다.
3. 위의 이미지 파일을 Photoshop CC 2021에서 DDS 포맷으로 변환한다.
4. 프로젝트의 AssetsList.txt에 사용할 에셋들의 이름을 전부 추가한다(model.mm, anim.mac, texture.dds).
5. 프로젝트의 MaterialList.txt에 사용할 마테리얼들을 추가한다(WallMat, wall_diffuse.dds, wall_normal.dds).
6. Importer로 위의 ~List.txt를 읽어서 해당 에셋들을 프로젝트의 리소스로 불러온다.
7. 손으로 직접 타이핑하여 Object를 추가하고, TransformComponent를 추가하고, 위치와 회전 등을 직접 적어 넣는다.
8. 사용할 에셋(모델이나 마테리얼 등)을 직접 손으로 적어 넣는다.

이상적인 컨버팅 과정은 다음과 같다.

1. 3ds Max에서 모델을 만들고 텍스처를 적용한 뒤에 Export한다.
2. 유니티에서 불러와서 배치한다.
3. 유니티에서 ObjectList로 Export 한다.
4. 프로젝트에서 로드를 하면 한 번에 읽을 수 있어야 한다.

FBX Parser의 기능을 Editor에 넣으면 Export 할 때 바로 바로 뺄 수 있지 않을까?

[3d max - Unity 호환이 잘 되는 유닛(Unit) 설정 하기 :: sunghye (tistory.com)](https://sunghye.tistory.com/6)

3ds Max에서 Export 할 때, Embed Media 옵션을 켜줘야 Material을 포함해서 Export 할 수 있다.

<img src="https://user-images.githubusercontent.com/21697638/162359296-6fb071b7-6957-44d6-afc0-cc5c90dcf1e9.png" width="70%" height="70%"></img>

-----------

### 22.04.12 - 22.04.25

#### 이번주 일정

* Unity로 Level Editor 만들기

### 2022.04.13

		//GetComponentsInChildren<>()는 자기 자신을 포함한 배열을 반환한다.
        Transform[] transforms = env.GetComponentsInChildren<Transform>();
        foreach(Transform child in transforms)
        {
	        if (child.name == "Env") continue;  // exclude self
			// ...
        }
        
        // 아래와 같이 사용해야 문제가 없다.
        Texture normal = mr.material.GetTexture("_BumpMap");
        if (normal) Debug.Log(normal.name);

### 2022.04.14

albedo Texture만 가진 instance material은 해당 albedo Texture의 이름으로 material이 생성되는데,
albedo와 normal texture를 둘 다 가지고 있는 경우엔 Material #25 같이 의미 없는 이름으로 생성됨

일단 이런 방식을 쓰려면 3ds max에서 모든 material에 diffuse, normal, specular를 다 넣어줘야 함
근데 뭐 텍스트로 적나 이렇게 하나 귀찮은건 똑같아보임
fresnel 값은 따로 지정하는 법을 모르겠음 아마 3ds max에서 하는거 같은데 거기서 정한 값이 unity에서 저장이 될 지도 미지수

unity에서 빈 오브젝트를 하나 만들고, BoxCollider Component를 넣어준 뒤에, collider의 center를 옮긴게 아니라 Object의 위치를 옮겨줬더니 box.center를 했더니 원점이 나온다.

FBX는 못 빼나? FBX를 .mm 확장자로 parse해서 뺄 수는 없나??
[유니티(c#)에서 c++ dll 사용하기 (1) : 네이버 블로그 (naver.com)](https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=seilgogo&logNo=220839071223)
를 쓰면 C++ 코드를 쓸 수 있는 모양
기존의 FBX parser를 유니티에 붙이고 다른거 export 할 때 같이 뺄 수 있으면 정말 편할 것 같다.

근데 일단 환경 메쉬랑 충돌체랑 마테리얼 빼는거 먼저 해보고 할 것.
[[유니티] 스크립트로 텍스트 파일 생성 및 쓰기, 저장 / 파일 입출력 (tistory.com)](https://88-it.tistory.com/181)
요 방법을 쓰면 텍스트 파일을 쓸 수 있을 것 같다.

material에 (instance) 붙어서 나오는게 넘 거슬린다. 심지어 앞에 공백도 한 칸 들어가있음.

<img src="https://user-images.githubusercontent.com/21697638/163302974-af077983-e3c0-4915-932b-b9992f60668e.png" width="70%" height="70%"></img>

<img src="https://user-images.githubusercontent.com/21697638/163303007-52edaf7f-ce3c-40c7-910f-540c82c2df5f.png" width="70%" height="70%"></img>
Material #25 라고 나오는건 중간의 공백을 없애서 Material#25로 변경했다.

조명 값도 빼야 함.

### 2022.04.15

조명값 뺐음

### 2022.04.16

<img src="https://user-images.githubusercontent.com/21697638/163656982-efcdc270-46f4-48b6-9cc4-05a4a627b832.png" width="70%" height="70%"></img>

해야 하는 것:

- 본 프로젝트에서 import 하기
- Door와 TargetBoard 같은 상호작용하는 특수 오브젝트도 배치하고 Export
- UI 렌더를 지금 후처리 들어가기 전에 해서 폰트에도 후처리 적용되는 문제가 있음, 이후로 옮겨야 함
- 체력, 장탄수, 뭐 그런거 보여주기

### 2022.04.18

18일부터 19일까지 다른 프로젝트 작업함

### 2022.04.22

다른 플레이어 입장
각 플레이어가 서로의 위치를 보고, 회전 정보 등을 얻음

좀 더 보완하면 좋은 점:

- 오브젝트 생성할 때, 스크립트 등으로 하면 좋겠다.
- 솔루션 내에서 소스코드랑 헤더를 필터로 관리중인데 이러면 한 폴더에 다 추가가 됨. 폴더 뷰? 로 관리를 하는 방법도 있음.
- 오브젝트 생성할 때, 중복되는 부분을 Initialize 함수 같은걸로 관리하면 좋겠다.
- UI 렌더를 후처리 전 단계에서 하고 있는것.

마음에 안드는 점:

- 루트시그니처가 너무 지저분함.
- 씬을 추가할 것을 고려하지 않고 만들어서 헤더 등이 지저분함.
- 멀티샘플링을 고려하지 않고 만들어서 텍스처 등의 자원 관리에 제한이 있음.
- 전체화면 전환?

실제로 해야 하는 점:

- export한 텍스트 파일을 읽어서 데이터화(아마 이걸 역직렬화라고 불렀던거 같음)
- 장전 추가

우선순위 리스트?
일단 하기로 한 것부터 다 쳐내기
- 장전
- 에디터 읽어오기

유니티에서 FBX파일을 씬에 넣고 FBX파일 안에 포함된 마테리얼로 즉석 생성을 하면
Standard 속성의 마테리얼이 instance로 생성되는데
이 마테리얼은 러프니스값을 디퓨즈 텍스처나 메탈릭 텍스처의 알파값에서 가져오고
러프니스값을 러프니스 텍스쳐에서 가져오려면 마테리얼의 속성을 Standard(specular setup)으로 바꿔야 함
근데 잘 생각해보면 나는 이미 dds 파일을 만들때 디퓨즈 텍스처의 알파채널에 러프니스 텍스처를 올려서 하나로 합쳐 쓰고 있었음

유니티에서 자동으로 마테리얼 잡는 것:
- 맥스에서 마테리얼 생성하고 디퓨즈 텍스처(러프니스 합침), 노멀 텍스처를 정해줌
- 그걸 익스포트함

### 2022.04.25

	- 플레이어의 권총에 들어가는 WeaponControllerComponent에 현재 총알 수, 최대 총알 수, 현재 장전중 여부 등의 값을 추가하였다.
	- 장전이란 기능이 가지는 의미를 그대로 수행한다(탄이 바닥나면 장전하고, R키를 누르면 몇 발이 남아있든 장전하고, 장전 중에는 사격할 수 없는).
	- Text 등의 UI가 후처리 전 단계에 그려져서 번지는 문제가 있었고 해결했다.
	- Player의 HP값을 받아올 수 있게 하였다. 이를 위해, HumanoidControllerComponent는 이제 Character class까지 다중상속한다.
	- Character는 maxHp, currHp, currHpPerMaxHp 등의 값을 Get()으로 가져올 수 있다.
	- 위의 모든 public으로 접근가능한 값은 TextUI로 출력할 수 있다.
	- Player의 HP와 Ammo를 Text로 출력한다.
	- 간단한 조준점을 추가하였다(애니메이션에 의해 총구가 향하는 방향이 매순간 바뀌므로 크게 의미는 없다).
	- 타이틀과 엔드 이미지를 추가하였다.
	- 에디터에서 환경 오브젝트와 마테리얼, 충돌체의 위치 등을 가져올 수 있게 하였다.
	- 그래서 이제 벽에 충돌이 된다!! 솔직히 충돌체 추가가 너무 필요했다.

<img src="https://user-images.githubusercontent.com/21697638/165242265-bce252db-23dd-43dd-8cdb-0d27a9652ea8.png" width="70%" height="70%"></img>



