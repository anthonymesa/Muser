file = open("muse_base.obj")
vertex_file = open("vertex.mus", "w")
texel_file = open("texel.mus", "w")
normal_file = open("normal.mus", "w")
face_file = open("face.mus", "w")


for line in file:
    if("v " in line):
        tmp = list(line)
        tmp = tmp[2:]
        vertex_file.write(''.join(tmp))
    elif("vt " in line):
        tmp = list(line)
        tmp = tmp[3:]
        texel_file.write(''.join(tmp))
    elif("vn " in line):
        tmp = list(line)
        tmp = tmp[3:]
        normal_file.write(''.join(tmp))
    elif("f " in line):
        tmp = list(line)
        tmp = tmp[2:]
        face_file.write(''.join(tmp))