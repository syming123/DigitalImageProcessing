import cv2
import numpy as np
import matplotlib.pyplot as plt

def  calculate_error_energy(gray1,gray2,d,half_width=1,half_height=1):
    pad_img1 = np.pad(gray1,((half_height,half_height),(half_width,half_width)))
    pad_img2 = np.pad(gray2,((half_height,half_height),(half_width,half_width)))
    height = gray1.shape[0]
    width = gray1.shape[1]
    E = np.zeros((height,width))
    EE = np.zeros((height,width))
    for i in range(height):
        for j in range(width):
            x = min(j+d,width-1)
            sum = ((pad_img1[i:i+2*half_height+1,x:x+2*half_width+1]
                    -pad_img2[i:i+2*half_height+1,j:j+2*half_width+1])**2).sum()
            E[i][j] = sum/(3*half_height*half_width)
    padE = np.pad(E,((half_height,half_height),(half_width,half_width)))
    for i in range(height):
        for j in range(width):
            sum = padE[i:i+2*half_height+1,j:j+2*half_width+1].sum()
            EE[i][j] = sum/(half_height*half_width)
    return EE

def disparity_map(img1,img2,dmax=40):
    B1,G1,R1 = img1[:,:,0],img1[:,:,1],img1[:,:,2]
    B2,G2,R2 = img2[:,:,0],img2[:,:,1],img2[:,:,2]
    height = B1.shape[0]
    width = B1.shape[1]
    dis = np.full((height,width),999999)
    D = np.zeros((height,width))
    for d in range(dmax):
        E1 = calculate_error_energy(B1,B2,d)
        E2 = calculate_error_energy(G1,G2,d)
        E3 = calculate_error_energy(R1,R2,d)
        E = E1 + E2 + E3
        for i in range(height):
            for j in range(width):
                if(dis[i][j] > E[i][j]):
                    dis[i][j] = E[i][j]
                    D[i][j] = d
    return D.astype(np.uint8),E


def filter_img(D,E,alpha=1.5):
    ve = alpha * E.mean()
    cv2.medianBlur(D,3)
    height = D.shape[0]
    width = D.shape[1]
    for i in range(height):
        for j in range(width):
            if(E[i][j] > ve):
                D[i][j] = 0
    return D


def calculate_deep(D,f=30,T=20):
    height = D.shape[0]
    width = D.shape[1]
    Deep = np.zeros((height,width))
    for i in range(height):
        for j in range(width):
            if(D[i][j] < 5):
                Deep[i][j] = 0
            else:
                Deep[i][j] = f*T/D[i][j]
    return Deep.astype(np.uint8)




img1 = cv2.imread("pics/5/view1m.png")
img2 = cv2.imread("pics/5/view5m.png")

D,E = disparity_map(img1,img2)
D = filter_img(D,E)

cv2.imshow("Disparity",D)
cv2.waitKey(0)

Deep = calculate_deep(D)

cv2.imshow("Deep",Deep)
cv2.waitKey(0)

ax=plt.figure().add_subplot(projection='3d')
xx,yy = np.meshgrid(range(Deep.shape[1]),range(Deep.shape[0]))
X,Y = xx.ravel(),yy.ravel()
bottom = np.zeros_like(X)
Z = Deep
Z = Z.ravel()
ax.bar3d(X,Y,bottom,1,1,Z,shade=True)
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')
plt.show()
cv2.waitKey(0)