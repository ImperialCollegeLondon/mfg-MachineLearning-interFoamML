// Stopping criteria
scalar areaLimit(readScalar(runTime.controlDict().lookup("areaLimit")));
scalar executionTimeLimit(readScalar(runTime.controlDict().lookup("executionTimeLimit")));

// access velocity field via reference
// const volVectorField& U
// (
//     mesh.lookupObject<volVectorField>("U")
// );
//
// const GeometricField<vector, fvPatchField, volMesh>& Ufield =
// this->db().objectRegistry::
// lookupObject<GeometricField<vector, fvPatchField, volMesh> >
// (
// "U"
// );


// access time variable
scalar t = runTime.time().value();

// find patch (integer) corresponding to inlet
label inletPatch = mesh.boundaryMesh().findPatchID("elbow_inlet");
if (inletPatch==-1)
{
  FatalError
    << "ERROR: Patch specified in controlDict codeExecute does not exist." << endl
    << exit(FatalError);
} else {
  Info << "INFO: referencing patch " << inletPatch << endl;
}

// TODO access to normal velocity is hard coded here
// TODO use dot product of U and normal vector to generalise
scalar inletVelocityMax = gMax(U.boundaryField()[inletPatch])[0];

Info << "inletVelocityMax: " << inletVelocityMax << endl;

const surfaceScalarField& faceArea = mesh.magSf();
scalar inletRate = gSum(faceArea.boundaryField()[inletPatch]*inletVelocityMax);
scalar cumulativeArea = inletRate * t;

scalar executionTime = runTime.elapsedCpuTime();
reduce(executionTime, sumOp<scalar>());

Info << "Test: executionTime: " << executionTime << endl;


Info << "Inlet rate: " << inletRate << endl;
Info << "Cummulative inflow area: " << cumulativeArea << endl;

if (cumulativeArea > areaLimit) {
  Info << "INFO: stopping simulation due to cumulative area limit" << endl;
  runTime.writeAndEnd();
  // return 0;
} else if (executionTime/Pstream::nProcs() > executionTimeLimit) {
  Info << "INFO: stopping simulation due to execution time limit" << endl;
  runTime.writeAndEnd();
  // return 0;
}
