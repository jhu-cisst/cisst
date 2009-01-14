void ExampleFrame(void) {
    // create 3 points
    vct3 point000(0.0, 0.0, 0.0);
    vct3 point100(3.0, 0.0, 0.0);
    vct3 point110(2.0, 3.2, 0.0);

    // create a normalized vector along the axis X
    // using methods
    vct3 axisX;
    axisX.DifferenceOf(point100, point000);
    axisX.Divide(axisX.Norm());

    // create a normalized vector along the axis Z
    // using operators '-' for difference,
    // '%' for cross product, and '/=' for in-place
    // elementwise division.
    vct3 tmp = point110 - point000;
    vct3 axisZ = axisX % tmp;
    axisZ /= axisZ.Norm();

    // Using named methods instead of operators
    vct3 axisY;
    axisY.CrossProductOf(axisZ, axisX);
    axisY.NormalizedSelf();

    /* three ways to display the result: */
    // 1. Just output a vector
    cout << "X: " << axisX << endl;
    // 2. Output vector component by index
    cout << "Y: " << axisY[0]
         << " " << axisY[1]
         << " " << axisY[2] << endl;
    // 3. Output vector component by ``name''
    cout << "Z: " << axisZ.X()
         << " " << axisZ.Y()
         << " " << axisZ.Z() << endl;
    /**/

    // create a rotation along axis "tmp"
    tmp.Assign(1.3, -0.3, 1.7);
    tmp.NormalizedSelf();
    vctMatRot3 rotation(vctAxAnRot3(tmp, 3.1415 / 2.0));

    /* two ways to apply the rotation
       to vectors: */
    vct3 newAxisX, newAxisY, newAxisZ;
    // 1. Using operator '*'
    newAxisX = rotation * axisX;
    // 2. Using named method ApplyTo
    rotation.ApplyTo(axisY, newAxisY);
    rotation.ApplyTo(axisZ, newAxisZ);

    /* verify that the transformed vectors are still
       an orthogonal basis.  Compute dot products
       in three ways. */
    // 1. Using operator * on two vectors
    double dotXY = newAxisX * newAxisY;
    // 2. Using global function vctDotProduct
    double dotYZ = vctDotProduct(newAxisY, newAxisZ);
    // 3. Using named method DotProduct
    double dotZX = newAxisZ.DotProduct(newAxisX);
    
    cout << "Dot products: " << dotXY << " "
         << dotYZ << " " << dotZX << endl;
    /**/

    // create a rigid transformation frame from
    // a rotation matrix and a translation vector
    vct3 translation(0.0, 3.2, 4.5);
    vctFrm3 frame(rotation, translation);
    // Apply the frame to a vector
    vct3 frameOnX = frame * axisX;
    cout << "Image of X: " << frameOnX << endl;
 
   // inverse of the frame
    vctFrm3 inverse;
    inverse.InverseOf(frame);
    // The product of a frame and its inverse
    // should be the identity (eye for rotation,
    // zero for translation).
    cout << "frame * inverse: " << endl << frame * inverse
         << endl;
    // Compare this with the actual identity frame
    cout << "Identity frame: " << endl
         << vctFrm3::Identity() << endl;
}
