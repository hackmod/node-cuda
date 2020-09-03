{
  "targets": [
    {
      "target_name": "cuda",
      "sources": [
        "src/common.cpp",
        "src/types.cpp",
        "src/bindings.cpp",
        "src/ctx.cpp",
        "src/device.cpp",
        "src/stream.cpp",
        "src/function.cpp",
        "src/mem.cpp",
        "src/module.cpp",
        "src/nvrtc.cpp"
        ],
      "include_dirs": [
                "<!(node -e \"require('nan')\")"
        ],
      'conditions': [
        [ 'OS=="mac"', {
          'libraries': ['-framework CUDA'],
          'include_dirs': ['/usr/local/include'],
          'library_dirs': ['/usr/local/lib']
        }],
        [ 'OS in "linux freebsd openbsd"', {
          'variables': {
            'NVIDIA_CUDA_SDK' : '<!(echo ${CUDA_PATH:-/usr/local/cuda})',
            'NVIDIA_CUDA_SDK_INCLUDE' : '<(NVIDIA_CUDA_SDK)/include',
            'NVIDIA_CUDA_SDK_LIB' : '<(NVIDIA_CUDA_SDK)/lib64',
          },
          'libraries': ['-lcuda'],
          'include_dirs': ['/usr/local/include', '<(NVIDIA_CUDA_SDK_INCLUDE)'],
          'library_dirs': ['/usr/local/lib', '<(NVIDIA_CUDA_SDK_LIB)']
        }],
        [ 'OS=="win"', {
          'conditions': [
            ['target_arch=="x64"',
              {
                'variables': { 'arch': 'x64' }
              }, {
                'variables': { 'arch': 'Win32' }
              }
            ],
          ],
          'variables': {
            'cuda_root%': '$(CUDA_PATH)'
          },
          'libraries': [
            '-l<(cuda_root)/lib/<(arch)/cuda.lib',
          ],
          "include_dirs": [
            "<(cuda_root)/include",
          ],
        }, {
          "include_dirs": [
            "/usr/local/cuda/include"
          ],
        }]
      ]
    }
  ]
}
