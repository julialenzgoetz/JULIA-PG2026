"""Gera os recursos vetoriais originais do cenário e o atlas de fonte.

Este script é somente uma ferramenta de autoria. O jogo carrega os PNGs já
incluídos; não precisa de Python. Requer Pillow e CairoSVG para regenerar.
O cachorro é um recurso separado, criado com ImageGen, e não é alterado aqui.
"""
from pathlib import Path
import math
import shutil
import cairosvg
from PIL import Image, ImageFont, ImageDraw

ROOT = Path(__file__).resolve().parents[1]
ASSETS = ROOT / "assets"
SOURCE = ASSETS / "vector_sources"
SOURCE.mkdir(exist_ok=True)

def svg_file(name, content, width=1280, height=720):
    svg = f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">{content}</svg>'
    (SOURCE / f"{name}.svg").write_text(svg, encoding="utf-8")
    cairosvg.svg2png(bytestring=svg.encode(), write_to=str(ASSETS / f"{name}.png"))

svg_file("sky", '''
<defs><linearGradient id="sky" x2="0" y2="1"><stop stop-color="#bce0db"/><stop offset="1" stop-color="#fbf2cf"/></linearGradient></defs>
<path fill="url(#sky)" d="M0 0h1280v720H0z"/>
<circle cx="1070" cy="168" r="92" fill="#fff9db" opacity=".3"/>
<circle cx="1070" cy="168" r="64" fill="#fff4c8"/>
<g fill="#fffbea" opacity=".78">
<path d="M95 167c-12-23 15-40 33-33 1-39 57-43 70-9 31-16 64 8 56 34 30-1 36 29 10 33H108c-28 0-33-22-13-25Z"/>
<path d="M505 97c-4-20 15-29 34-26 8-31 47-32 58-7 24-8 43 10 40 28 25-4 33 20 12 24H514c-21-1-26-16-9-19Z"/>
<path d="M804 226c-8-19 9-32 28-25 5-30 43-33 56-9 23-11 49 5 44 26 27-1 28 24 8 28H813c-26 0-29-17-9-20Z"/>
</g>
<g stroke="#71948a" stroke-width="3" fill="none" stroke-linecap="round"><path d="M742 151q10-10 20 0 10-10 20 0"/><path d="M794 130q7-7 14 0 7-7 14 0"/></g>''')

svg_file("hills", '''
<path d="M0 440V363Q155 278 335 382Q470 215 655 351Q838 240 1000 368Q1170 330 1280 363V720H0Z" fill="#93bbaa"/>
<path d="M0 426Q190 332 384 445Q620 324 824 437Q1030 336 1280 426V720H0Z" fill="#729c84"/>
<path d="M0 487Q180 432 346 486Q507 395 717 475Q957 412 1100 472Q1190 451 1280 487V720H0Z" fill="#537e67"/>''')

trees = []
for x,y,scale in [(100,488,.90),(430,497,.67),(910,488,1.16),(1160,495,.62)]:
    trees.append(f'''<g transform="translate({x} {y}) scale({scale})">
    <path d="M-10 0l5-130h15L18 0Z" fill="#6d7f60"/>
    <path d="M2-86l-36-33M7-65l34-35" stroke="#6d7f60" stroke-width="9" stroke-linecap="round"/>
    <path d="M-64-92c-48-5-58-63-17-84-6-38 35-74 69-53 31-25 71-3 76 28 55 4 67 61 34 86-30 23-61 11-79 14-27 16-51 14-83 9Z" fill="#6e9977"/>
    <path d="M-82-172c1-31 36-51 66-34 27-20 63-4 65 24 29 3 37 21 39 35-32-12-51-1-74 10-38-19-65 0-96-35Z" fill="#82ad86"/>
    </g>''')
trees.append('<path d="M0 520Q35 484 77 514Q124 476 171 517Q211 496 239 522Q278 473 324 517Q350 501 388 526Q447 485 483 523Q539 495 561 525Q592 480 643 517Q699 502 731 523Q779 484 830 519Q879 475 919 515Q955 494 998 522Q1054 477 1103 518Q1144 491 1181 518Q1246 483 1280 520V720H0Z" fill="#9ab775"/>')
svg_file("trees", ''.join(trees))

ground='''<path fill="#779851" d="M0 535h1280v185H0z"/><path fill="#b5cc7a" d="M0 539h1280v16H0z"/><path fill="#e5c691" d="M0 555h1280v165H0z"/><path fill="#f3d9aa" d="M0 559h1280v8H0z"/><path fill="#d2b483" d="M0 682h1280v38H0z"/>'''
for i in range(28):
    x=(i*191+23)%1280; y=578+(i*31)%99
    ground+=f'<path d="M{x} {y}h{7+i%7}" stroke="#c8a977" stroke-width="3" stroke-linecap="round" opacity=".6"/>'
for i in range(16):
    x=(i*127+15)%1280
    ground+=f'<path d="M{x} 544l-5-10m5 10 2-13m-2 13 8-7" stroke="#6e934c" stroke-width="2" fill="none"/>'
    if i%3==0:
        ground+=f'<circle cx="{x+18}" cy="539" r="4" fill="#fff5c5"/><circle cx="{x+18}" cy="539" r="1.8" fill="#e3b147"/>'
svg_file("ground",ground)

# Atlas de itens: 4 colunas, 2 linhas, células de 128 px.
# 0..3: petisco girando. 4 tronco, 5 pedra, 6 poça, 7 coração.
items=[]
for i,scale in enumerate([1,.80,.46,.80]):
    items.append(f'''<g transform="translate({i*128+64} 64) scale({scale} 1) rotate(-18)">
    <path d="M-30-13c-22-24-43 10-22 22-9 24 22 36 31 12h44c9 24 40 12 31-12 21-12 0-46-22-22l-9 11h-34Z" fill="#f3c668" stroke="#865b37" stroke-width="5" stroke-linejoin="round"/>
    <path d="M-18 4h35" stroke="#ffe8a3" stroke-width="5" stroke-linecap="round"/>
    </g>''')
items.append('''<g transform="translate(0 128)"><path d="M15 69Q8 38 32 37l67-8q18 0 19 23v38q0 22-20 24l-68 1q-16 0-16-22Z" fill="#a77345" stroke="#654934" stroke-width="5"/><ellipse cx="29" cy="76" rx="20" ry="38" fill="#e4b97d" stroke="#654934" stroke-width="5"/><ellipse cx="29" cy="76" rx="10" ry="23" fill="none" stroke="#b7814f" stroke-width="3"/><path d="M53 48l45-5M55 67l41-4M53 91l51-3" stroke="#805837" stroke-width="4" stroke-linecap="round"/><path d="M68 34l-4-18 12 2 6 15" fill="#a77345" stroke="#654934" stroke-width="4"/></g>''')
items.append('''<g transform="translate(128 128)"><path d="M12 108l12-50 24-24 35 5 23 31 11 38Z" fill="#8d9b9c" stroke="#4c6264" stroke-width="5" stroke-linejoin="round"/><path d="M24 58l31 2 28-21M55 60l19 48M55 60l51 10" fill="none" stroke="#6e8387" stroke-width="4"/><path d="M28 58l20-19 28 2-22 14Z" fill="#bdccbf"/></g>''')
items.append('''<g transform="translate(256 128)"><path d="M9 88q-4-15 23-20 20-25 43-7 35-1 44 21 8 30-46 30-57 13-64-9Z" fill="#6cb8c0" stroke="#397f89" stroke-width="4"/><path d="M26 86q29-13 52-5m8 9 12-1" fill="none" stroke="#c5eeeb" stroke-width="5" stroke-linecap="round"/></g>''')
items.append('''<g transform="translate(384 128)"><path d="M64 112C52 102 12 78 12 48C12 17 47 10 64 36C81 10 116 17 116 48C116 78 76 102 64 112Z" fill="#d96a57" stroke="#9b4c42" stroke-width="5" stroke-linejoin="round"/><path d="M31 40q9-8 17 0" stroke="#f8b49c" stroke-width="7" stroke-linecap="round" fill="none"/></g>''')
svg_file('items',''.join(items),512,256)

font_path=ROOT/'third_party/font/DejaVuSans-Bold.ttf'
font_path.parent.mkdir(exist_ok=True)
if not font_path.exists():
    shutil.copy('/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf',font_path)
font=ImageFont.truetype(str(font_path),84)
atlas=Image.new('RGBA',(2048,2048),(255,255,255,0))
advances=[]
for code in range(256):
    char=chr(code)
    x=(code%16)*128; y=(code//16)*128
    cell=Image.new('RGBA',(128,128),(255,255,255,0))
    draw=ImageDraw.Draw(cell)
    if code>=32 and code!=127:
        draw.text((4,0),char,font=font,fill=(255,255,255,255),anchor='la')
    atlas.paste(cell,(x,y))
    advances.append(str(float(font.getlength(char))))
atlas.save(ASSETS/'font.png')
(ASSETS/'font.metrics').write_text('\n'.join(advances)+'\n')
print('Generated sky, hills, trees, ground, item sheet and font atlas.')
