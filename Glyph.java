public class Glyph {
    public final int codePoint;
    public final int xBearing;
    public final int yBearing;
    public final int width;
    public final int height;
    public final int xAdvance;

    public Glyph(int codePoint, int xBearing, int yBearing, int width, int height, int xAdvance) {
        this.codePoint = codePoint;
        this.xBearing = xBearing;
        this.yBearing = yBearing;
        this.width = width;
        this.height = height;
        this.xAdvance = xAdvance;
    }
}
