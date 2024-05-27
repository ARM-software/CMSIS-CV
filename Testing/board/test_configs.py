# Solution and build file for all
# the tests
# It is a pair : csolution target type and AVH identification
# AVH identification is used to find the executable
# and the configuration file
# #
# Compiler version can also be used like for instance : AC6@6.22.0
compiler={
    'AC6':[
      ("VHT-Corstone-300","CS300"),
      # No core config for running on AVH so it is a board run
      #("MPS3-Corstone-300",),
      #("VHT-M4","M4FP"),
      ("VHT-M7","M7DP"),
    ],
    #'CLANG':[
    #  ("VHT-Corstone-300","CS300"),
    #],
    'GCC':[
      ("VHT-Corstone-300","CS300"),
    ]
}

compiler={
    'AC6':[
      ("VHT-Corstone-300","CS300"),
    ],
}