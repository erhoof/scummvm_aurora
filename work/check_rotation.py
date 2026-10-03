from pathlib import Path
import re, subprocess
source = Path('backends/graphics/windowed.h').read_text()
methods = '\n'.join(re.search(r'Common::Point '+name+r'\([^\n]+\) const \{.*?\n\t\}', source, re.S).group(0) for name in ('convertVirtualToWindow','convertWindowToVirtual'))
harness = r'''
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#define AURORA_OS 1
namespace Common { enum RotationMode {kRotationNormal=0,kRotation90=90,kRotation180=180,kRotation270=270}; struct Point {int x,y; Point(int a,int b):x(a),y(b){}}; }
template<class T> T CLIP(T a,T lo,T hi){return std::max(lo,std::min(a,hi));}
void error(const char *s){std::puts(s); std::abort();}
struct Rect {int left,top,right,bottom; int width()const{return right-left;} int height()const{return bottom-top;}};
struct Area {Rect drawRect;int width,height;};
struct Mapping {Common::RotationMode _rotationMode;Area _activeArea;
''' + methods + r'''
};
int main(){
  int checked=0;
  for(int angle: {0,90,180,270}) for(int scale: {1,2,3}) {
    bool quarter=angle==90||angle==270;
    Mapping m{static_cast<Common::RotationMode>(angle),{{13,27,13+(quarter?480:640)*scale,27+(quarter?640:480)*scale},640,480}};
    for(int y=0;y<480;++y)for(int x=0;x<640;++x){
      auto window=m.convertVirtualToWindow(x,y);
      auto game=m.convertWindowToVirtual(window.x,window.y);
      if(game.x!=x||game.y!=y){std::printf("FAIL angle=%d scale=%d game=%d,%d -> window=%d,%d -> game=%d,%d\n",angle,scale,x,y,window.x,window.y,game.x,game.y);return 1;}
      ++checked;
    }
  }
  std::printf("PASS: %d round trips across all four rotations and three scales, including nonzero viewport offsets\n",checked);
}
'''
Path('/tmp/scummvm-rotation-check.cpp').write_text(harness)
subprocess.run(['g++','-std=c++11','-O2','/tmp/scummvm-rotation-check.cpp','-o','/tmp/scummvm-rotation-check'],check=True)
subprocess.run(['/tmp/scummvm-rotation-check'],check=True)
