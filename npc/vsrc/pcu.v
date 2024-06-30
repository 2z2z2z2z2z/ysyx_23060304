module pcu (
    input wire clk,
    input wire rst,

    output reg [31:0] pc_o
);
    
    always @(posedge clk or posedge rst) begin
        if (rst) begin
            pc_o <= 32'h8000_0000;
        end
        else begin
            pc_o <= pc_o + 32'h4;
        end
    end
endmodule
