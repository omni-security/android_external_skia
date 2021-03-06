/*
 * Copyright 2013 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrPathProcessor_DEFINED
#define GrPathProcessor_DEFINED

#include "GrPrimitiveProcessor.h"

struct PathBatchTracker {
    GrGPInput fInputColorType;
    GrGPInput fInputCoverageType;
    GrColor fColor;
    bool fUsesLocalCoords;
};

/*
 * The path equivalent of the GP.  For now this just manages color. In the long term we plan on
 * extending this class to handle all nvpr uniform / varying / program work.
 */
class GrPathProcessor : public GrPrimitiveProcessor {
public:
    static GrPathProcessor* Create(GrColor color,
                                   const SkMatrix& viewMatrix = SkMatrix::I(),
                                   const SkMatrix& localMatrix = SkMatrix::I()) {
        return SkNEW_ARGS(GrPathProcessor, (color, viewMatrix, localMatrix));
    }

    void initBatchTracker(GrBatchTracker*, const GrPipelineInfo&) const override;

    bool canMakeEqual(const GrBatchTracker& mine,
                      const GrPrimitiveProcessor& that,
                      const GrBatchTracker& theirs) const override;

    const char* name() const override { return "PathProcessor"; }

    GrColor color() const { return fColor; }
    const SkMatrix& viewMatrix() const { return fViewMatrix; }
    const SkMatrix& localMatrix() const { return fLocalMatrix; }


    void getInvariantOutputColor(GrInitInvariantOutput* out) const override;
    void getInvariantOutputCoverage(GrInitInvariantOutput* out) const override;

    bool willUseGeoShader() const override { return false; }

    virtual void getGLProcessorKey(const GrBatchTracker& bt,
                                   const GrGLSLCaps& caps,
                                   GrProcessorKeyBuilder* b) const override;

    virtual GrGLPrimitiveProcessor* createGLInstance(const GrBatchTracker& bt,
                                                     const GrGLSLCaps& caps) const override;

private:
    GrPathProcessor(GrColor color, const SkMatrix& viewMatrix, const SkMatrix& localMatrix);

    /*
     * CanCombineOutput will return true if two draws are 'batchable' from a color perspective.
     * TODO is this really necessary?
     */
    static bool CanCombineOutput(GrGPInput left, GrColor lColor, GrGPInput right, GrColor rColor) {
        if (left != right) {
            return false;
        }

        if (kUniform_GrGPInput == left && lColor != rColor) {
            return false;
        }

        return true;
    }

    static bool CanCombineLocalMatrices(const GrPrimitiveProcessor& l,
                                        bool leftUsesLocalCoords,
                                        const GrPrimitiveProcessor& r,
                                        bool rightUsesLocalCoords) {
        if (leftUsesLocalCoords != rightUsesLocalCoords) {
            return false;
        }

        const GrPathProcessor& left = l.cast<GrPathProcessor>();
        const GrPathProcessor& right = r.cast<GrPathProcessor>();
        if (leftUsesLocalCoords && !left.localMatrix().cheapEqualTo(right.localMatrix())) {
            return false;
        }
        return true;
    }

    bool hasExplicitLocalCoords() const override { return false; }

    GrColor fColor;
    const SkMatrix fViewMatrix;
    const SkMatrix fLocalMatrix;

    typedef GrPrimitiveProcessor INHERITED;
};

#endif
