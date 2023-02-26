# Resting Grounds

> Resting place for shelved ideas. May they one day rise again as something new.

Below is a brief description of what each folder contains:

* `assets`: a general asset library intended to allow the conversion of meshes, textures,
  etc. into a format we can easily load back in and upload to the GPU. It is greatly
  inspired by [vk-guide](https://vkguide.dev/docs/extra-chapter/asset_system/) though it
  does contain significant deviations from it. It is mostly untested code, and I strongly
  suspect there's a lot of cases that need to be considered further.
* `kass`: an asset conversion system that relies on `assets`. It is mostly skeleton code,
  though it does contain an example on how to use NVTT 3. **Note:** in Windows, this
  requires `cudart64_11.dll` to be copied over. See `Findnvtt.cmake` for more details on
  ideas on how this could be accomplished more easily.
