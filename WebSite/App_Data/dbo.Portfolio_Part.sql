CREATE TABLE [dbo].[Portfolio_Part] (
    [Id]                   INT        IDENTITY (1, 1) NOT NULL,
    [portfolio_id]         INT        NULL,
    [financial_product_id] INT        NULL,
    [part] FLOAT NULL, 
    [date] FLOAT NULL, 
    PRIMARY KEY CLUSTERED ([Id] ASC),
    CONSTRAINT [FK_Portfolio_Part_ToPortfolio] FOREIGN KEY ([portfolio_id]) REFERENCES [dbo].[Portfolio] ([Id]),
    CONSTRAINT [FK_Portfolio_Part_ToFinancial_Product] FOREIGN KEY ([financial_product_id]) REFERENCES [dbo].[Financial_Product] ([Id])
);

