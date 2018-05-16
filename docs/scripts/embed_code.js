var entityMap = {
    "&": "&amp;",
    "<": "&lt;",
    ">": "&gt;",
    '"': '&quot;',
    "'": '&#39;',
    "/": '&#x2F;'
};

function escapeHtml(string) {
    return String(string).replace(/[&<>"'\/]/g, function(s)
                                  {return entityMap[s];});
}


var code = document.getElementsByClassName("code")
var pre;
for (var i = 0; i < code.length; i++) {
	pre=document.createElement("pre");
	pre.innerHTML = '<code class="prettyprint lang-cpp">'
				  + escapeHtml(code[i].innerHTML)
				  + '</code>';
	code[i].parentNode.insertBefore(pre,code[i]);
}
