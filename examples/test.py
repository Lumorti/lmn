import numpy as np

# Output with less precision
np.set_printoptions(precision=3, suppress=True)

# Define the dimension d
d = 4

# Given a list of vectors and eigenvalues, find the operator that has them as eigenvectors
def findOperator(eigenvecs, eigenvals):
    n = len(eigenvecs)
    M = np.zeros((d, d), dtype=complex)
    for i in range(n):
        M += eigenvals[i] * np.outer(eigenvecs[i], eigenvecs[i].conj())
    return M

# Given an operator, find the eigenvectors
def findEigenvectors(M):
    eigenvals, eigenvecs = np.linalg.eig(M)
    return eigenvecs.T

# Create the X operator
def genX():
    X = np.zeros((d, d), dtype=complex)
    for i in range(d - 1):
        X[i + 1, i] = 1
    X[0, d - 1] = 1
    return X
X = genX()

# Create the Y operator
def genY(theta=1):
    Y = np.zeros((d, d), dtype=complex)
    omega = np.exp(theta * 2.0 * np.pi * 1j / float(d))
    for i in range(d - 1):
        Y[i + 1, i] = 1j * np.power(omega, i)
    Y[0, d - 1] = 1j * np.power(omega, d - 1)
    return Y
Y = genY(1)

# Create the Z operator
def genZ(theta=1):
    Z = np.zeros((d, d), dtype=complex)
    omega = np.exp(theta * 2.0 * np.pi * 1j / float(d))
    for i in range(d):
        Z[i, i] = np.power(omega, i)
    return Z
Z = genZ(1)

# Create the H operator
def genH(theta=1):
    H = np.zeros((d, d), dtype=complex)
    omega = np.exp(theta * 2.0 * np.pi * 1j / float(d))
    factor = 1.0 / np.sqrt(d)
    for i in range(d):
        for j in range(d):
            H[i, j] = factor * np.power(omega, i * j)
    return H

# The identity
I = np.eye(d, dtype=complex)

# The operator pool
operatorPool = [I, genX(), genY(), genZ(), genH()]
stringPool = ["I", "X", "Y", "Z", "H"]
numOps = len(operatorPool)

# Given a number, using ternary representation to generate the matrix
def matrixFromNumber(num):
    M = I
    while num > 0:
        rem = num % numOps
        M = operatorPool[rem] @ M
        num = num // numOps
    return M

# Given a number, using ternary representation to generate the string
def stringFromNumber(num):
    string = ""
    while num > 0:
        rem = num % numOps
        string = stringPool[rem] + string
        num = num // numOps
    return string

vals = [1, -1, 1j, -1j]
M_1 = np.array([
    [0.5, 0.5, 0.5, 0.5],
    [0.5, 0.5, -0.5, -0.5],
    [0.5, -0.5, -0.5, 0.5],
    [0.5, -0.5, 0.5, -0.5]
])
M_2 = 0.5 * np.array([
    [1, -1, -1j, -1j],
    [1, -1, 1j, 1j],
    [1, 1, 1j, -1j],
    [1, 1, -1j, 1j]
])
M_3 = 0.5 * np.array([
    [1, -1j, -1j, -1],
    [1, -1j, 1j, 1],
    [1, 1j, 1j, -1],
    [1, 1j, -1j, 1]
])
M_4 = 0.5 * np.array([
    [1, -1j, -1, -1j],
    [1, -1j, 1, 1j],
    [1, 1j, -1, 1j],
    [1, 1j, 1, -1j]
])

opToFind1 = findOperator(M_1, vals)
opToFind2 = findOperator(M_2, vals)
opToFind3 = findOperator(M_3, vals)
opToFind4 = findOperator(M_4, vals)

print("Original vectors:")
print(M_1)
print("Operator:")
print(opToFind1)
print("Eigenvectors:")
print(findEigenvectors(opToFind1))
print("Eigenvalues:")
print(np.linalg.eig(opToFind1)[0])
print()

print("Original vectors:")
print(M_2)
print("Desired eigenvalues:")
print(vals)
print("Operator:")
print(opToFind2)
print("Eigenvectors:")
print(findEigenvectors(opToFind2))
print("Eigenvalues:")
print(np.linalg.eig(opToFind2)[0])
print("Back as an operator:")
print(findOperator(findEigenvectors(opToFind2), np.linalg.eig(opToFind2)[0]))
print()

print("Original vectors:")
print(M_3)
print("Operator:")
print(opToFind3)
print("Eigenvectors:")
print(findEigenvectors(opToFind3))
print()

print("Original vectors:")
print(M_4)
print("Operator:")
print(opToFind4)
print("Eigenvectors:")
print(findEigenvectors(opToFind4))
print()

exit()

for matNum in range(1, 100):
    M = matrixFromNumber(matNum)
    print(matNum, stringFromNumber(matNum))
    print(M)
    if np.allclose(M, opToFind):
        print(matNum)
        print("Found it!")
        break

exit()

minOverall = 1000
maxNum = 30
# for matNum1 in range(1, maxNum):
for matNum1 in range(1):
    # for matNum2 in range(1, maxNum):
    for matNum2 in range(1):
        # for matNum3 in range(1, maxNum):
        for matNum3 in range(1):
            # for matNum4 in range(1, maxNum):
            for matNum4 in range(1):

                # Define the M matrix list
                M = []
                M.append(Z)
                M.append(X)
                M.append(Z@X)
                M.append(Z@Z@X)
                # M.append(Z@(Z@(Z@X)))
                # M.append(Z@(Z@(Z@(X@(Z@(Z@Z))))))
                # M.append(matrixFromNumber(matNum1))
                # M.append(matrixFromNumber(matNum2))
                # M.append(matrixFromNumber(matNum3))
                # M.append(matrixFromNumber(matNum4))
                n = len(M)

                # Print the eigenvectors
                # print()
                eigenvecs = [None] * n
                for i in range(n):
                    eigenvecs[i] = np.linalg.eig(M[i])[1]
                    # for j in range(d):
                        # print(eigenvecs[i][:, j])

                # Calculate the inner products
                maxError = 0
                for i in range(n):
                    for j in range(i+1, n):
                        for k in range(d):
                            for l in range(d):
                                innerProduct = np.dot(eigenvecs[i][:, k], eigenvecs[j][:, l])
                                innerProduct = np.abs(innerProduct) - 1.0 / np.sqrt(d)
                                if innerProduct > maxError:
                                    maxError = innerProduct

                print("Max error: ", maxError)
                minOverall = min(minOverall, maxError)

print("Min overall: ", minOverall)

