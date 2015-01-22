CREATE TABLE [dbo].[Value] (
    [Id]                   INT        IDENTITY (1, 1) NOT NULL,
    [value]                FLOAT (53) NULL,
    [date]                 FLOAT (53) NULL,
    [financial_product_id] INT        NULL,
    [price_portfolio_id]   INT        NULL,
    [error_modele_id]      INT        NULL,
    [ic] FLOAT NULL, 
    PRIMARY KEY CLUSTERED ([Id] ASC),
    CONSTRAINT [FK_Value_ToFinancial_Product] FOREIGN KEY ([financial_product_id]) REFERENCES [dbo].[Financial_Product] ([Id]),
    CONSTRAINT [FK_Value_ToModele] FOREIGN KEY ([error_modele_id]) REFERENCES [dbo].[Modele] ([Id]),
    CONSTRAINT [FK_Value_ToPortfolio] FOREIGN KEY ([price_portfolio_id]) REFERENCES [dbo].[Portfolio] ([Id])
);

