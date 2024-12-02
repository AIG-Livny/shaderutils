#!/usr/bin/python3

def get_config() -> 'mapyr.Config':
    tc = mapyr.Config()
    tc.MINIMUM_REQUIRED_VERSION = '0.6.0'
    return tc

def get_project(name:str) -> 'mapyr.Project|None':
    if name not in ['main','debug']:
        return None

    p = mapyr.create_c_project(
        'libshaderutils.a',
        private_config={
            'SOURCES':['shaderutils.c'],
            'CFLAGS':['-Ofast','-flto'] if name == 'main' else ['-g','-O0'],
            'LINK_FLAGS':['-flto'] if name == 'main' else [],
        },
        export_config={
            'INCLUDE_DIRS':['.'],
            'LIBS':['shaderutils'],
            'LIB_DIRS':['.'],
        },
        subprojects=[
            mapyr.get_module('../stringlib/build.py').get_project(name),
            mapyr.get_module('../fileutils/build.py').get_project(name),
        ]
    )

    return p

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
    mapyr.process(get_project,get_config)