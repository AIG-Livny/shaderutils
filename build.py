#!/usr/bin/python3

import copy

def tool_config() -> "mapyr.ToolConfig":
    tc = mapyr.ToolConfig()
    tc.MINIMUM_REQUIRED_VERSION = '0.4.5'

def config() -> list["mapyr.ProjectConfig"]:

    result = []

    # Debug config
    debug = mapyr.ProjectConfig()

    debug.OUT_FILE = "libshaderutils.a"
    debug.SRC_DIRS = ["."]
    debug.SUBPROJECTS = [
        "../stringlib",
        "../fileutils",
    ]
    debug.CFLAGS    = ["-g","-O0"]
    debug.GROUPS = ['DEBUG']

    result.append(debug)

    # Release config
    release = copy.deepcopy(debug)
    release.CFLAGS    = ["-Ofast","-flto"]
    release.LINK_EXE_FLAGS = ["-flto"]

    release.GROUPS = ['RELEASE']

    result.append(release)
    return result

#-----------FOOTER-----------
# https://github.com/AIG-Livny/mapyr.git
try:
    import mapyr
except:
    import requests, os
    os.makedirs(f'{os.path.dirname(__file__)}/mapyr',exist_ok=True)
    with open(f'{os.path.dirname(__file__)}/mapyr/__init__.py','+w') as f:
        f.write(requests.get('https://raw.githubusercontent.com/AIG-Livny/mapyr/master/__init__.py').text)
    import mapyr

if __name__ == "__main__":
    mapyr.process(config)