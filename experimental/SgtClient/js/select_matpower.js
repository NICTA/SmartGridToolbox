var selector = $("#select_matpower");
var files = $.getJSON('http://localhost:34568/matpower_files/', function(files) {
    for (i = 0; i < files.length; ++i)
    {
        selector.append("<option>" + files[i] + '</option>');
    }
});

selector.change(
    function() {
        var file = $(this).find("option:selected").text();
        var url = 'http://localhost:34568/networks/' + file;
        var json = JSON.stringify({"matpower_filename": file});
        $.ajax({
            url: url,
            type: 'PUT',
            contentType: "application/json",
            data: json,
            success: function(result) {
                loadNetwork(file);
            }
        });
    }
);
