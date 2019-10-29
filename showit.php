<?php declare(strict_types=1); ?>
<!DOCTYPE html><html><body>
<script> function printImage() {window.print();} </script>
<div>
  <?php echo "Title: " . $_POST["title"]; ?>
  <?php echo " &nbsp  page: " . $_POST["pagenumb"] . " &nbsp "; ?>
  <button onclick="printImage()">Print</button>
  <form action="/showit.php" method="post" target="_self">
    <input type="hidden" name="title" value=<?php echo $_POST["title"];?> />
    <input type="hidden" name="cloudPortal" value=<?php echo $_POST["cloudPortal"];?> />
    <input type="hidden" name="bucketName" value=<?php echo $_POST["bucketName"];?> />
    <button type="submit" formmethod="post" name="pagenumb"
      value=<?php echo $_POST["pagenumb"] + 1;?>> Forward </button>
    <button type="submit" formmethod="post" name="pagenumb"
      value=<?php echo $_POST["pagenumb"];?>> Refresh </button>
    <button type="submit" formmethod="post" name="pagenumb"
      value=<?php echo (max(0, $_POST["pagenumb"] - 1));?>> Backwrd </button>
  </form>
</div>

<canvas id="theCanvas" style="border:1px solid #d3d3d3"></canvas>
<?php    echo ('
<script>
var cnvs = document.getElementById("theCanvas");
var ctx = cnvs.getContext("2d");
const WIDE = 1001; const HIGH = 1001;
cnvs.width = WIDE; cnvs.height = HIGH;
function interp(seq) { //generate canvas contents
  // following defs must mirror the generator code
  var next = 1;
  const MOVE_TO = next++;
  const LINE_TO = next++;
  const HLINE   = next++;
  const RLINE   = next++; // relative right line 1..256 pixels
  const LLINE   = next++; // left
  const VLINE   = next++;
  const ULINE   = next++; // up
  const DLINE   = next++; // down
  const STROKE  = next++;
  const COLOR   = next++;
  const RECT    = next++;
  const FONT    = next++;
  const WRITE   = next++;
  const ENDL    = next++; // next text line

  function getCoord() {return ((seq.charCodeAt(j++) << 8) + seq.charCodeAt(j++));}
  function getxy()    { getx(); gety();}
  function getx()     { x = getCoord(); return(x);}
  function gety()     { y = HIGH - getCoord(); return(y);}
  function getw()     { return(getCoord());}
  function geth()     { return(-getCoord());}
  function getd()     { d = seq.charCodeAt(j++); d = d ? d : 256; return(d);}
  function getColor() {
    color = (seq.charCodeAt(j++) << 16);
    color += (seq.charCodeAt(j++) << 8) + seq.charCodeAt(j++);
  }
  function getstring() {
    it = ""; // hopefully there is a better way
    while (seq.charCodeAt(j++)) it += seq.slice(j - 1, j);
    return(it);
  }
  var len = seq.length;
  var x = 0, y = HIGH, d, color = 0, tx = 0, it = "", j = 0;
  ctx.font = font;
  var txtheight = 24; // TODO adjust per font
  while (j < len) {
    var theCmd = seq.charCodeAt(j++);
    switch (theCmd) {
    case MOVE_TO: ctx.moveTo(tx = getx(), gety()); break;
    case LINE_TO: ctx.lineTo(tx = getx(), gety()); break;
    case HLINE:   ctx.lineTo(tx = getx(), y); break;
    case RLINE:   ctx.lineTo(tx = x += getd(), y); break;
    case LLINE:   ctx.lineTo(tx = x -= getd(), y); break;
    case VLINE:   ctx.lineTo(x, gety()); break;
    case ULINE:   ctx.lineTo(x, y += getd()); break;
    case DLINE:   ctx.lineTo(x, y -= getd()); break;
    case RECT:    ctx.fillRect(x, y, getw(), geth()); break;
    case FONT:    font = getstring(); ctx.font = font; break;
    case WRITE:
      it = getstring();
      var measure = ctx.measureText(it);
      ctx.fillText(it, tx, y);
      tx += measure.width;
      break;
    case ENDL:    y += getd(); tx = x; break;
    case COLOR:
      ctx.stroke();
      ctx.beginPath();
      ctx.moveTo(x, y);
      getColor();
      var s = "#" + color.toString(16).padStart(6, "0");
      ctx.fillStyle = s;
      ctx.strokeStyle = s;
      break;
    default:
    } // switch(theCmd)
  } // while (j < len)
  ctx.stroke();
} // function interp()

var chars = "');  // fetch the base64 encoded data

function zpad($what, $ln = 3) {
  $sPrintfStr = '%0';
  $sPrintfStr .= (int)$ln;
  $sPrintfStr .= 's';
  return(sprintf($sPrintfStr, $what));
}

$filename = "http://";
$filename .= $_POST["cloudPortal"]; // storage.googleapis.com/
$filename .= $_POST["bucketName"];  // xpoz
$filename .= "/" . $_POST["title"] . ".p";
$filename .= zpad($_POST["pagenumb"], 3);
readfile($filename);
echo ('";

var bytes = atob(chars);
var font = "20px Courier New";
interp(bytes);
</script>');
//echo ($filename);
?></body></html>

