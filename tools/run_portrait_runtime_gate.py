"""Join the production resource table/tail, portraits and first image consumer.

Host bootstrap/file access are bounded services. Upload entry throws, so no GPU
success is supplied. CPU image parsing runs separately with the original files.
"""
import argparse
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import zipfile

from run_particle_control_smoke import function

ROOT = Path(__file__).resolve().parents[1]


def extract(path, signatures):
    source = (ROOT / path).read_text()
    return '\n'.join(function(source, signature) for signature in signatures)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cxx', default=shutil.which('g++'))
    parser.add_argument('--assets', type=Path, required=True)
    parser.add_argument('--obb', type=Path, help='cross-check portrait bytes against original OBB')
    args = parser.parse_args()
    if not args.cxx:
        parser.error('specify --cxx')
    if args.obb:
        material = (ROOT / 'src/engine/render/ResourceMaterial.cpp').read_text()
        paths = re.findall(r'String\("(data/textures/[^"]+)"', material)
        assert len(paths) == 152
        present = missing = 0
        with zipfile.ZipFile(args.obb) as archive:
            names = set(archive.namelist())
            for base in paths:
                for suffix in ('', '_ipad', '_ipad_1440', '_ipad_large'):
                    relative = base + suffix + '.aei'
                    entry = '47947_GOF2CONTENT_ETC/assets/' + relative
                    local = args.assets / relative
                    assert local.is_file() == (entry in names), relative
                    if entry in names:
                        assert local.read_bytes() == archive.read(entry), relative
                        present += 1
                    else:
                        missing += 1
        assert (present, missing) == (602, 6)
        print('Original OBB: 602 portrait files byte-identical to parser inputs, six absent', flush=True)
    table = (ROOT / 'src/engine/render/BuildResourceList.cpp').read_text()
    prelude = table[table.index('namespace AbyssEngine {\n    struct Resource {'):
                    table.index('\n\nvoid BuildResourceList')]
    start = table.index('    Resource *resources[2488];')
    end_marker = '    loadPortraits(engine);'
    end = table.index(end_marker, start) + len(end_marker)
    # Preserve the entire table and tail, then the real loadPortraits call.
    registration = prelude + '\nvoid registerPortraitResources(AbyssEngine::Engine *engine) {\n'
    registration += 'using namespace AbyssEngine;\nauto *canvas = engine->appManager->paintCanvas;\n'
    registration += table[start:end] + '\n}\n'
    bodies = extract('src/game/core/String.cpp', (
        'String::~String()', 'char *String::GetAEChar() const', 'String::String() :',
        'String::String(const char *cstr, bool reverse)',
        'String::String(const String &other, bool reverse)',
        'void String::Set(const char *s)', 'void String::Set(const unsigned short *s)',
        'int String::GetStringLength(const char *s)',
        'String &String::operator+=(const String &other)'))
    bodies += '\nnamespace AbyssEngine {\n' + extract('src/engine/core/AbyssEngine.cpp', (
        'String operator+(const String &a, const String &b)',
        'int ImageCreateFromFile(Engine *engine, const char *path, Image **out) {',
        'void ImageRelease(Image **slot) {'))
    bodies += extract('src/engine/render/ResourceTexture.cpp', (
        'ResourceTexture::ResourceTexture(const char *name, float value)',
        'ResourceTexture::ResourceTexture(const String &name, float value)',
        'ResourceTexture::~ResourceTexture()'))
    bodies += extract('src/engine/render/ResourceMaterial.cpp', (
        'ResourceMaterial::ResourceMaterial(uint16_t texId, uint16_t texId2, BlendMode blend)',
        'ResourceMaterial::ResourceMaterial(uint16_t texId, BlendMode blend)'))
    bodies += extract('src/engine/render/ResourceMesh.cpp', (
        'ResourceMesh::ResourceMesh(const char *name, unsigned short id, bool flag)',
        'ResourceMesh::~ResourceMesh()')) + '\n}\n'
    bodies += extract('src/engine/render/ResourceMaterial.cpp', (
        'void loadPortraits(AbyssEngine::Engine *engine)',))
    bodies += extract('src/engine/render/PaintCanvas.cpp', (
        'void paintcanvas_ext_add_resource(void *resource, void *resources)',
        'void *PaintCanvas::FindResource(unsigned short id)',
        'void PaintCanvas::AddResource(AbyssEngine::Resource *resource)',
        'void PaintCanvas::SetResourceList(AbyssEngine::Resource * const * list, unsigned int count)',
        'void PaintCanvas::TextureCreate(unsigned short resId, void (*callback)',
        'void PaintCanvas::TextureCreate(unsigned short id, unsigned int &out, bool flag)',
        'void PaintCanvas::Image2DCreate(unsigned short resId, unsigned int &out)'))
    bodies += extract('src/engine/render/ImageFactory.cpp', (
        'void *ImageFactory::loadImage(int row, int col, int frameBase)',))
    fixture = (ROOT / 'tools/portrait_runtime_gate.cpp').read_text()
    fixture = fixture.replace('// RESOURCE_REGISTRATION', registration)
    fixture = fixture.replace('// PRODUCTION_BODIES', bodies)
    with tempfile.TemporaryDirectory(prefix='portrait-runtime-', dir=ROOT / '_work') as temp:
        source = Path(temp) / 'portrait_runtime.cpp'
        source.write_text(fixture)
        for opt in ('-O0', '-O2'):
            exe = Path(temp) / f'portrait_runtime_{opt[1:]}.exe'
            subprocess.run([args.cxx, '-std=gnu++17', opt, '-g0', '-Isrc', str(source),
                            '-o', str(exe)], cwd=ROOT, check=True)
            subprocess.run([str(exe), str(args.assets.resolve())], cwd=ROOT, check=True)


if __name__ == '__main__':
    main()
