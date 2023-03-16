import numpy as np

from numba  import jit
RADIUS = {"H": 0.1, "C": 0.2, "N": 0.2, "O": 0.2,"F": 0.2, "S": 0.2, "Cl": 0.2, "Br": 0.2, "I": 0.2}

def read_xyz(filename):
    xyz = np.loadtxt(filename, skiprows=2, usecols=(1, 2, 3))
    elements = np.loadtxt(filename, skiprows=2, usecols=(0), dtype=str)
    return elements, xyz


def determine_bond(coordinates, elements, max_bond=1.6, min_bond=0):
    num_atoms = len(elements)
    bonds = {}
    for i in range(num_atoms):
        for j in range(i+1, num_atoms):
            distance = np.linalg.norm(coordinates[i] - coordinates[j])
            if distance > min_bond and distance < max_bond:
                bonds[(i, j)] = 1
    return bonds

def render_mol(coordinates, elements, bonds):
    WIDHT = 200
    HEIGHT = 50
    X_max = np.max(coordinates[:, 0])
    Z_max = np.max(coordinates[:, 2])
    X_min = np.min(coordinates[:, 0])
    Z_min = np.min(coordinates[:, 2])
    X_range = X_max - X_min
    Z_range = Z_max - Z_min
    X_scale = WIDHT / X_range
    Z_scale = HEIGHT / Z_range
    scale = min(X_scale, Z_scale)*0.6
    coordinates = coordinates * scale + np.array([WIDHT/2, 0, HEIGHT/2])
    print(coordinates)
    #define the grid
    grid = np.zeros((WIDHT,HEIGHT), dtype=str)
    # determine the bond
    for i in range(WIDHT):
        for j in range(HEIGHT):
            grid[i,j] = " "
            if_draw = False
            depth = 0
            for bond in bonds:

                bond_vector = np.array([coordinates[bond[1],0] - coordinates[bond[0],0], coordinates[bond[1],2] - coordinates[bond[0],2]])
                distance_vector = np.array([i - coordinates[bond[0],0], j - coordinates[bond[0],2]])
                cos_angle = np.dot(distance_vector, bond_vector)/(np.linalg.norm(distance_vector)*np.linalg.norm(bond_vector))
                sin_angle = 1 - cos_angle**2
                distance = np.linalg.norm(distance_vector)*np.sqrt(sin_angle)
                distance_vector_1 = np.array([i - coordinates[bond[1],0], j - coordinates[bond[1],2]])


                if distance < 1.0 and np.dot(distance_vector,bond_vector)>0 and np.dot(distance_vector_1,bond_vector)<0:
                    grid[i,j] = "-"
                #    print("angle1 = ", angle1, "angle2 = ", angle2, "angle3 = ", angle3, "angle4 = ", angle4)
            for k in range(len(elements)):
                if (i - coordinates[k,0])**2 + 3*(j - coordinates[k,2])**2 < (RADIUS[elements[k]]*scale*0.8)**2 and not if_draw:
                    grid[i,j] = elements[k]
                    if_draw = True
                    depth = coordinates[k,1]
                    #print(f"i = {i}, j = {j}, k = {elements[k]}, depth = {depth}")
                elif 3*(i - coordinates[k,0])**2 + 3*(j - coordinates[k,2])**2 < (RADIUS[elements[k]]*scale*0.8)**2 and if_draw:
                    if coordinates[k,1] > depth:
                        grid[i,j] = elements[k]
                        depth = coordinates[k,1]
            # draw the bond


    return grid

def rotate(coordinates, angle_x, angle_y, angle_z):
    #rotate around x axis
    R_x = np.array([[1, 0, 0], [0, np.cos(angle_x), -np.sin(angle_x)], [0, np.sin(angle_x), np.cos(angle_x)]])
    #rotate around y axis
    R_y = np.array([[np.cos(angle_y), 0, np.sin(angle_y)], [0, 1, 0], [-np.sin(angle_y), 0, np.cos(angle_y)]])
    #rotate around z axis
    R_z = np.array([[np.cos(angle_z), -np.sin(angle_z), 0], [np.sin(angle_z), np.cos(angle_z), 0], [0, 0, 1]])
    R = R_x @ R_y @ R_z
    coordinates = coordinates @ R
    return coordinates




if __name__ == "__main__":
    elemnts, xyz = read_xyz("CH4.xyz")
    
    bond = determine_bond(xyz, elemnts)
    grid = render_mol(xyz, elemnts,bond)
    print(bond)
#    xyz = rotate(xyz, 10, 0, 0)
#    grid = render_mol(xyz, elemnts)
#benchmark the time
    import time
    start = time.time()
    for i in range(2):
        grid = render_mol(xyz, elemnts,bond)
    end = time.time()
    print(end - start)

    
